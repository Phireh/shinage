#include "shinage.h"


int main(int argc, char *argv[])
{
  /* Command args handling */
  for (int i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
    {
      printf("Usage:\n");
      printf("\t-h | --help:\tPrint help\n");
      printf("\t-v | --version:\tPrint version number\n");
    }
    if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
    {
      printf("SHINAGE version %s", version);
      return 0;
    }
  }

  /* X11 Window creation */
  if (!(x11_display = XOpenDisplay(NULL)))
  {
    log_err("Could not open X11 display\n");
    return 0;
  }

  x11_screen = DefaultScreenOfDisplay(x11_display);
  x11_screen_id = DefaultScreen(x11_display);

  int x11_window_border_color = BlackPixel(x11_display, x11_screen_id);
  int x11_window_background_color = WhitePixel(x11_display, x11_screen_id);
  // TODO: Check return status of this
  x11_window = XCreateSimpleWindow(x11_display,
                                   RootWindowOfScreen(x11_screen),
                                   x11_window_x_coord,
                                   x11_window_y_coord,
                                   x11_window_width,
                                   x11_window_height,
                                   x11_window_border_size,
                                   x11_window_border_color,
                                   x11_window_background_color);

  Atom wmDeleteMessage = XInternAtom(x11_display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(x11_display, x11_window, &wmDeleteMessage, 1);
                                   
  XSelectInput(x11_display, x11_window, KeyPressMask | KeyReleaseMask | KeymapStateMask);

  XStoreName(x11_display, x11_window, "Shinage");
  
  XClearWindow(x11_display, x11_window);
  XMapRaised(x11_display, x11_window);

  /* Window message loop */
  XEvent x11_event;
  KeySym keysym = 0;
  main_loop_state_t main_loop_state = RUNNING;
  while (main_loop_state == RUNNING)
  {
      // TODO: Sleep so we don't burn the CPU
      XNextEvent(x11_display, &x11_event);

      switch (x11_event.type)
      {
      case KeymapNotify:
          XRefreshKeyboardMapping(&x11_event.xmapping);
          break;

      case KeyPress:
          keysym = XLookupKeysym(&x11_event.xkey, 0);
          switch (keysym)
          {
          case XK_Escape:
              main_loop_state = FINISHED;
              break;
          }
          break;

      case ClientMessage:
          /* Petition to close window by the window manager. See Xlib's ICCCM docs */
          if (x11_event.xclient.data.l[0] == (long int)wmDeleteMessage)
              main_loop_state = FINISHED;
          break;
              
          
      }
  }

  /* Cleanup */  
  XDestroyWindow(x11_display, x11_window);
  XCloseDisplay(x11_display);
  return 1;  
}
