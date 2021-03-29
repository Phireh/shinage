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

  x11_window = XCreateSimpleWindow(x11_display, RootWindowOfScreen(x11_screen), 0, 0, 320, 200, 1, BlackPixel(x11_display, x11_screen_id), WhitePixel(x11_display, x11_screen_id));

  XClearWindow(x11_display, x11_window);
  XMapRaised(x11_display, x11_window);

  /* Window message loop */
  XEvent x11_event;
  while (1)
  {
    XNextEvent(x11_display, &x11_event);
  }

  /* Cleanup */  
  XDestroyWindow(x11_display, x11_window);
  XFree(x11_screen);
  XCloseDisplay(x11_display);
  return 1;  
}
