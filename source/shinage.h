#ifndef SHINAGE_H
#define SHINAGE_H

/* CRT includes */
#include <stdio.h>
#include <string.h>

/* X11 server includes:
   requires linking with -lX11 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* OpenGL related includes */



char *version = "0.1";

/* X11 globals */
Display *x11_display;
Window x11_window;
Screen *x11_screen;
int x11_screen_id;

/* Convenience macros */
#define log_err(str, ...) fprintf(stderr, "[ERROR] (%s:%d: %s" str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
