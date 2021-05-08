#ifndef SHINAGE_H
#define SHINAGE_H

/* CRT includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <errno.h>

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

/* Internal includes */
#include "shinage_debug.h"
#include "shinage_math.h"
#include "shinage_camera.h"
#include "shinage_input.h"
#include "shinage_opengl_signatures.h"
#include "shinage_shaders.h"

/* shinage_text also includes ft2build.h and FT_FREETYPE_H */
#include "shinage_text.h"

/* Types */
typedef enum {
    PAUSED,
    RUNNING,
    FINISHED,
    TOTAL
} main_loop_state_t;

typedef struct {
    vec3f position;
} entity_t;

/* Misc. typedefs */
typedef unsigned int    uint;
typedef      uint8_t   uint8;
typedef     uint16_t  uint16;
typedef     uint32_t  uint32;
typedef     uint64_t  uint64;
typedef       int8_t    int8;
typedef      int16_t   int16;
typedef      int32_t   int32;
typedef      int64_t   int64;


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

/* Misc. globals for testing */
int framecount = 0;
entity_t test_triangle;
entity_t test_pyramid;

/* Timing globals */
float target_s_per_frame = (1.0f/60.0f); // ~60 fps
/* We start up the program with the assumption that Vsync is ON.
   TODO: Check if this is a good assumption on all X11 WMs */
bool vsync = true;


/* OpenGL defines */
// NOTE: This is some magic so we are able to create OpenGL contexts with an exact GL version
#define GLX_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB		0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);


/* OpenGL globals */
GLXContext glx_context;

char *simple_color_vertex_shader =
    "#version 150\n"      \
    "in vec3 position;\n" \
    "in vec3 vColor;\n" \
    "out vec3 fColor;\n" \
    "uniform mat4 modelMatrix;\n"  \
    "uniform mat4 viewMatrix;\n"  \
    "uniform mat4 projMatrix;\n"  \
    "void main() {\n" \
    "gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);\n" \
    "fColor = vColor;\n" \
    "}";

char *simple_color_fragment_shader =
    "#version 150\n" \
    "out vec4 out_color;\n" \
    "in vec3 fColor;\n" \
    "void main() {\n" \
    "out_color = vec4(fColor, 1.0);\n" \
    "}";


/* Misc. inline functions */

static inline void move_entity(entity_t *e, float x_offset, float y_offset, float z_offset)
{
    e->position.x += x_offset;
    e->position.y += y_offset;
    e->position.z += z_offset;
}


/* TODO: Find if we get a monotonic clock that gives us a growing value with just one
   call to initilize the clock */

static inline double get_current_time()
{
    long            ns; // Nanoseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ns = spec.tv_nsec;

    double currentSeconds = s + ns / 1.0e9;

    return currentSeconds;
}

static inline float get_delta_time()
{
    static double time_lf = 0;
    if (!time_lf)
        time_lf = get_current_time();

    double time_cf = get_current_time();
    float delta = time_cf - time_lf;

    time_lf = time_cf;
    return delta;
}

/* Convenience functions for checking current window dimentions.
   Currently only used inside shinage_text */

int get_window_width()
{
    return x11_window_width;
}

int get_window_height()
{
    return x11_window_height;
}


/* Functions */
int check_for_glx_extension(char *extension, Display *display, int screen_id);
//void draw_gl_triangle(void);
void draw_gl_pyramid(float *colours);
void draw_gl_cube(float *colours);
int link_gl_functions(void);
void test_entity_logic(player_input_t *input, entity_t *e);
void test_cube_logic(player_input_t *input, entity_t *e);
void log_debug_cpu_computed_vertex_positions(float *vertices, uint count, uint dims);
void draw_bouncing_cube_scene(void);
void draw_static_cubes_scene(uint segments);
int set_pointer_state(player_input_t *input, pointer_state_t new_state);
int set_vsync(bool new_state);

#endif
