#ifndef SHINAGE_H
#define SHINAGE_H

/* CRT includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* X11 server includes:
   requires linking with -lX11 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* OpenGL related includes:
   requires linking with -lGL */
#include <GL/glx.h>

/* Types */
typedef enum {
    PAUSED,
    RUNNING,
    FINISHED,
    TOTAL
} main_loop_state_t;


char *version = "0.1";

/* X11 globals */
Display *x11_display;
Window x11_window;
Screen *x11_screen;
int x11_screen_id;
int x11_window_x_coord = 0;
int x11_window_y_coord = 0;
int x11_window_width = 320;
int x11_window_height = 200;
unsigned int x11_window_border_size = 1;


/* OpenGL defines */
// NOTE: This is some magic so we are able to create OpenGL contexts with an exact GL version
#define GLX_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB		0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

/* OpenGL globals */
GLXContext glx_context;

/* Convenience macros */
// NOTE: ##__VA_ARGS__ is a compiler extension and may not be portable. Maybe check for compiler defs here.
#define log_err(str, ...) fprintf(stderr, "[ERROR] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_debug(str, ...) fprintf(stderr, "[DEBUG] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_info(str, ...) fprintf(stderr, "[INFO] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/* Functions */
int check_for_glx_extension(char *extension, Display *display, int screen_id);

#endif
