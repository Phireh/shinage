#ifndef SHINAGE_H
#define SHINAGE_H

/* CRT includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* X11 server includes:
   requires linking with -lX11 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>

/* OpenGL related includes:
   requires linking with -lGL */
#include <GL/glx.h>
#include <GL/glext.h>

/* Internal includes */
#include "shinage_debug.h"
#include "shinage_math.h"
#include "shinage_camera.h"
#include "shinage_input.h"


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
camera_t main_camera = {
    .pos = { .x = 0.0f, .y = 0.0f, .z = -1.0f },
    .target = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
    .fov = 90.0f,
    .up = { .x = 0.0f, .y = 1.0f, .z = 0.0f },
    .near = 0.01f,
    .far = 10.0f,
    .viewport_w = 320,
    .viewport_h = 200
};


/* OpenGL defines */
// NOTE: This is some magic so we are able to create OpenGL contexts with an exact GL version
#define GLX_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB		0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

/* OpenGL function signatures */
PFNGLUSEPROGRAMPROC              glUseProgram;
PFNGLGETSHADERIVPROC             glGetShaderiv;
PFNGLSHADERSOURCEPROC            glShaderSource;
PFNGLCOMPILESHADERPROC           glCompileShader;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
PFNGLCREATESHADERPROC            glCreateShader;
PFNGLCREATEPROGRAMPROC           glCreateProgram;
PFNGLDELETESHADERPROC            glDeleteShader;
PFNGLGETPROGRAMIVPROC            glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
PFNGLATTACHSHADERPROC            glAttachShader;
PFNGLLINKPROGRAMPROC             glLinkProgram;
PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
PFNGLGENBUFFERSPROC              glGenBuffers;
PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
PFNGLBINDBUFFERPROC              glBindBuffer;
PFNGLBUFFERDATAPROC              glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLUNIFORM3FPROC               glUniform3f;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
    
/* OpenGL globals */
GLXContext glx_context;

char *test_vertex_shader =
    "#version 150\n"      \
    "in vec2 position;\n" \
    "uniform vec3 translation;\n" \
    "uniform mat4 viewMatrix;\n"  \
    "uniform mat4 projMatrix;\n"  \
    "void main() {\n" \
    "mat4 modelMatrix = mat4(1.0);\n"
    "modelMatrix[3] = vec4(translation, 1.0);"
    "gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(position, 0.0, 1.0);\n" \
    "}";

char *test_fragment_shader =
    "#version 150\n" \
    "out vec4 out_color;\n" \
    "void main() {\n" \
    "out_color = vec4(1.0, 1.0, 1.0, 1.0);\n" \
    "}";


char *pyramid_vertex_shader =
    "#version 150\n"      \
    "in vec3 position;\n" \
    "in vec3 vColor;\n" \
    "out vec3 fColor;\n" \
    "uniform vec3 translation;\n" \
    "uniform mat4 viewMatrix;\n"  \
    "uniform mat4 projMatrix;\n"  \
    "void main() {\n" \
    "mat4 modelMatrix = mat4(1.0);\n" \
    "modelMatrix[3] = vec4(translation, 1.0);\n" \
    "gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);\n" \
    "fColor = vColor;\n" \
    "}";

char *pyramid_fragment_shader =
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



/* Functions */
int check_for_glx_extension(char *extension, Display *display, int screen_id);
void draw_gl_triangle(void);
void draw_gl_pyramid(void);
void draw_gl_cube(void);
int link_gl_functions(void);
unsigned int make_gl_program(char *vertex_shader_source, char *fragment_shader_source);
unsigned int build_shader(char *source, int type);
void test_entity_logic(player_input_t *input, entity_t *e);

#endif
