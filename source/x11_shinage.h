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
#include <sys/inotify.h>
#include <unistd.h>
#include <fcntl.h>

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

char *single_light_vertex_shader_path = "./shaders/single_light_simple_shader.vert";
char *single_light_fragment_shader_path = "./shaders/single_light_simple_shader.frag";

unsigned int simple_color_program = 0;

/* Linux related globals */
char inotify_buffer[sizeof(struct inotify_event) + NAME_MAX + 1];
int inotify_fd;

int inotify_code_wd;     // for game code
int inotify_shaders_wd;  // for shaders

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

int load_game_code(game_code_t *game_code)
{
    static void *library_handle = NULL;
    if (library_handle)
        dlclose(library_handle);

    library_handle = dlopen("./shinage_game.so", RTLD_NOW);
    void *address = dlsym(library_handle, "game_update");
    if (!address)
    {
        log_err("Error loading game code: %s\n", dlerror());
        return 0;
    }
    game_code->game_update = address;
    address = dlsym(library_handle, "game_render");
    if (!address)
    {
        log_err("Error loading game code: %s\n", dlerror());
        return 0;
    }
    game_code->game_render = address;

    return 1;
}

void build_programs(game_state_t *state)
{
    state->simple_color_program = make_gl_program(simple_color_vertex_shader_path, simple_color_fragment_shader_path);
    state->single_light_program = make_gl_program(single_light_vertex_shader_path, single_light_fragment_shader_path);
}

/* Reloads the dynamic part of game code if shinage_game.so was edited.
   Reloads shaders if files inside ./shaders are edited
 */
int reload_game_code(game_code_t *game_code, game_state_t *state)
{
    ssize_t ret;
    while ((ret = read(inotify_fd, inotify_buffer, sizeof(inotify_buffer))) != -1)
    {

        struct inotify_event *event = (struct inotify_event*) inotify_buffer;
        if (event->wd == inotify_code_wd && event->mask & IN_CLOSE_WRITE && !strcmp(event->name, "shinage_game.so"))
        {
            log_info("File %s has changed, reloading game code", event->name);
            load_game_code(game_code);
        }
        if (event->wd == inotify_shaders_wd && event->mask & IN_CLOSE_WRITE)
        {
            log_info("File %s has changed, reloading shaders", event->name);
            build_programs(state);
        }

    }
    if (ret == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        // TODO: Error handling
        log_err("Error reading inotify fd");
#define CASE_STATEMENT(x) case x: { log_err(#x); } break
        switch (errno)
        {
            CASE_STATEMENT(EBADF);
            CASE_STATEMENT(EFAULT);
            CASE_STATEMENT(EINTR);
            CASE_STATEMENT(EINVAL);
            CASE_STATEMENT(EIO);
            CASE_STATEMENT(EISDIR);
        }
    }
#undef CASE_STATEMENT

    // Return 1 on code reload
    return ret > 0 ? 1 : 0;
}


/* Functions */
int check_for_glx_extension(char *extension, Display *display, int screen_id);
int link_gl_functions(void);
void log_debug_cpu_computed_vertex_positions(float *vertices, uint count, uint dims);
int set_vsync(bool new_state);
void build_programs();

#endif
