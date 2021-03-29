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

  /* GLX initialization */
  GLint glx_attr[] = {
      GLX_RGBA,
      GLX_DOUBLEBUFFER,
      GLX_DEPTH_SIZE, 24,
      GLX_STENCIL_SIZE, 8,
      GLX_RED_SIZE, 8,
      GLX_GREEN_SIZE, 8,
      GLX_BLUE_SIZE, 8,
      GLX_SAMPLE_BUFFERS, 0,
      GLX_SAMPLES, 0,
      None
  };
  XVisualInfo *glx_visual = glXChooseVisual(x11_display, x11_screen_id, glx_attr);

  if (!glx_visual)
  {
      log_err("Unable to create GLX virtual");      
      return 1;
  }

  GLint major_glx_version, minor_glx_version;
  glXQueryVersion(x11_display, &major_glx_version, &minor_glx_version);
  log_info("GLX version %d.%d", major_glx_version, minor_glx_version);
  

  XSetWindowAttributes x11_window_set_attr;
  x11_window_set_attr.border_pixel = BlackPixel(x11_display, x11_screen_id);
  x11_window_set_attr.background_pixel = WhitePixel(x11_display, x11_screen_id);
  x11_window_set_attr.override_redirect = True;
  x11_window_set_attr.colormap = XCreateColormap(x11_display, RootWindow(x11_display, x11_screen_id), glx_visual->visual, AllocNone);

  // NOTE: We can't use XCreateSimpleWindow if we need GLX/OpenGL
  x11_window = XCreateWindow(x11_display,
                             RootWindowOfScreen(x11_screen),
                             x11_window_x_coord,
                             x11_window_y_coord,
                             x11_window_width,
                             x11_window_height,
                             x11_window_border_size,
                             glx_visual->depth,
                             InputOutput,
                             glx_visual->visual,
                             CWBackPixel | CWColormap | CWBorderPixel | CWEventMask,
                             &x11_window_set_attr);

  /* OpenGL context creation */
  // TODO: Error handling
  glx_context = glXCreateContext(x11_display, glx_visual, NULL, GL_TRUE);
  glXMakeCurrent(x11_display, x11_window, glx_context);
  
  log_info("Starting OpenGL version %s", glGetString(GL_VERSION));
  
  // Register WM_DELETE_WINDOW so we can handle the window manager's close request in our message dispatch                           
  Atom wmDeleteMessage = XInternAtom(x11_display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(x11_display, x11_window, &wmDeleteMessage, 1);
                                   
  XSelectInput(x11_display, x11_window, KeyPressMask | KeyReleaseMask | KeymapStateMask | ExposureMask);

  // Set title name for our open window
  XStoreName(x11_display, x11_window, "shinage");
  
  XClearWindow(x11_display, x11_window);

  // Actually show the window
  XMapRaised(x11_display, x11_window);

  // How large is the window
  XWindowAttributes x11_window_attr;
  XGetWindowAttributes(x11_display, x11_window, &x11_window_attr);

  /* Window message loop */
  XEvent x11_event;
  KeySym keysym = 0;
  main_loop_state_t main_loop_state = RUNNING;
  while (main_loop_state == RUNNING)
  {
      /* Event handling */
      
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
          {
              main_loop_state = FINISHED;
              log_debug("Closing window by window manager's request");
          }
          break;

      case Expose:
          /* The window has been resized */
          XGetWindowAttributes(x11_display, x11_window, &x11_window_attr);
          log_debug("Window resized; Window width %d; Window height %d", x11_window_attr.width, x11_window_attr.height);
          break;
      }

      /* Rendering */

      // NOTE: This is just for testing that OpenGL actually works
      
  }

  /* Cleanup */  
  XDestroyWindow(x11_display, x11_window);
  XCloseDisplay(x11_display);
  return 1;  
}
