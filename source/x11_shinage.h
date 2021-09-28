#ifndef X11_SHINAGE_H
#define X11_SHINAGE_H

/* CRT includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <errno.h>

/* Dynamic code injection includes */
#include <dlfcn.h>

/* X11 server includes:
   requires linking with -lX11 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>

/* X11 extensions:
   requires linking with -lXfixes
   TODO: Figure out how ubiquitous these are,
   and if we should use them on release
*/
#include <X11/extensions/Xfixes.h>

/* OpenGL related includes:
   requires linking with -lGL */
#include <GL/glx.h>
#include <GL/glext.h>

/* Internal and cross-platform includes */
#include "shinage_common.h"

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

/* OpenGL globals */
char *simple_color_vertex_shader_path = "./shaders/simple_color.vert";
char *simple_color_fragment_shader_path = "./shaders/simple_color.frag";
unsigned int simple_color_program = 0;

/* Convenience functions for checking current window dimentions.
   Currently only used inside shinage_text */

int get_window_width()
{
    return x11_window_width;
}


int set_pointer_state(player_input_t *input, pointer_state_t new_state)
{
    /* Grab the pointer */
    if (new_state == GRABBED)
    {
        int ret = XGrabPointer(x11_display,
                               x11_window,
                               true,
                               0, // TODO: Figure out what this value does
                               GrabModeAsync,
                               GrabModeAsync,
                               x11_window,
                               None,
                               CurrentTime);
        if (ret == BadCursor)
        {
            log_debug("Error trying to grab mouse: BadCursor");
            return 0;
        }
        else if (ret == BadValue)
        {
            log_debug("Error trying to grab mouse: BadValue");
            return 0;
        }
        else if (ret == BadWindow)
        {
            log_debug("Error trying to grab mouse: BadWindow");
            return 0;
        }
        else
        {
            XFixesHideCursor(x11_display, x11_window);
            input->pointer_state = new_state;

            /* Don't move the in-game pointer from the center of the screen */
            input->cursor_x = (int)(x11_window_width/2);
            input->cursor_y = (int)(x11_window_height/2);

            if (XWarpPointer(x11_display, None, x11_window, 0, 0, 0, 0, x11_window_width/2, x11_window_height/2) == BadWindow)
            {
                log_debug("Error trying to move pointer to center of window: BadWindow");
            }

            /* Flush the event queue */
            XSync(x11_display, True);
            return 1;
        }
    }
    /* Ungrab the pointer */
    if (new_state == NORMAL)
    {
        XUngrabPointer(x11_display, CurrentTime);
        XFixesShowCursor(x11_display, x11_window);
        input->pointer_state = new_state;
        return 1;
    }

    return 0;
}

/* Functions */
int check_for_glx_extension(char *extension, Display *display, int screen_id);
int link_gl_functions(void);
void log_debug_cpu_computed_vertex_positions(float *vertices, uint count, uint dims);
int set_vsync(bool new_state);
void build_programs();

#endif
