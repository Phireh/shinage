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
#include <GL/glext.h>

/* Internal includes */
#include "shinage_math.h"
#include "shinage_camera.h"

/* Convenience macros */
// NOTE: ##__VA_ARGS__ is a compiler extension and may not be portable. Maybe check for compiler defs here.
#define log_err(str, ...) fprintf(stderr, "[ERROR] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_debug(str, ...) fprintf(stderr, "[DEBUG] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_info(str, ...) fprintf(stderr, "[INFO] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

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

/* Misc. globals for testing */
entity_t test_triangle;

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

    
/* OpenGL globals */
GLXContext glx_context;

char *test_vertex_shader =
    "#version 150\n"      \
    "in vec2 position;\n" \
    "uniform vec3 translation;\n" \
    "void main() {\n" \
    "mat4 modelMatrix = mat4(1.0);\n"
    "modelMatrix[3] = vec4(translation, 1.0);"
    "gl_Position = modelMatrix*vec4(position, 0.0, 1.0);\n" \
    "}";

char *test_fragment_shader =
    "#version 150\n" \
    "out vec4 out_color;\n" \
    "void main() {\n" \
    "out_color = vec4(1.0, 1.0, 1.0, 1.0);\n" \
    "}";

/* Input handling macros */
#define CTRL_MOD_KEY (1 << 0)
#define SHIFT_MOD_KEY (1 << 1)
#define ALT_MOD_KEY (1 << 2)

/* NOTE: Adding more descriptive aliases for X11's button macros.
   There are some more buttons not described by the X11 header that
   we might want to define here, too.
 */

// These are just unsigned ints
#define LEFT_MOUSE_BUTTON        Button1 // 1
#define RIGHT_MOUSE_BUTTON       Button3 // 3
#define WHEEL_PRESS_MOUSE_BUTTON Button2 // 2
#define WHEEL_UP_MOUSE_BUTTON    Button4 // 4
#define WHEEL_DOWN_MOUSE_BUTTON  Button5 // 5

typedef enum {
    PLAYER_1 = 0, PLAYER_2 = 1, PLAYER_3 = 2, PLAYER_4 = 3, MAX_PLAYERS = 4
} player_idx_t;


/* Input handling types */
// TODO: complete input
// TODO: extra data structure for input remapping info

typedef union {
    struct {        
        bool                up;
        bool              down;
        bool              left;
        bool             right;
        bool  mouse_left_click;
        bool mouse_right_click;        
        /* NOTE: What are these coords relative to? Whatever it is, it should be
         consistent between platforms */
        unsigned int  cursor_x;
        unsigned int  cursor_y;
        bool               alt;
        bool              ctrl;
        bool             shift;
    };
} player_input_t;

/* Input handling globals */
uint8_t modifier_keys = 0;
char mod_key_str[64];

/* NOTE: For performance reasons we want to access these as pointers and swap them around
   each frame instead of accessing them directly. Henceforth the _ prefix */   

player_input_t _curr_frame_input[MAX_PLAYERS]; 
player_input_t _last_frame_input[MAX_PLAYERS];

player_input_t *curr_frame_input = &_curr_frame_input[0];
player_input_t *last_frame_input = &_last_frame_input[0];
const player_input_t empty_player_input = {};

/* Input handling inlines */
static inline bool  ctrl_key_is_set() { return modifier_keys &  CTRL_MOD_KEY; };
static inline bool shift_key_is_set() { return modifier_keys & SHIFT_MOD_KEY; };
static inline bool   alt_key_is_set() { return modifier_keys &   ALT_MOD_KEY; };

static inline void  set_ctrl_key() { modifier_keys |=  CTRL_MOD_KEY; };
static inline void set_shift_key() { modifier_keys |= SHIFT_MOD_KEY; };
static inline void   set_alt_key() { modifier_keys |=   ALT_MOD_KEY; };

static inline void  unset_ctrl_key() { modifier_keys &=  !CTRL_MOD_KEY; };
static inline void unset_shift_key() { modifier_keys &= !SHIFT_MOD_KEY; };
static inline void   unset_alt_key() { modifier_keys &=   !ALT_MOD_KEY; };

static inline char *mod_key_str_prefix()
{
    char *s = mod_key_str;
    *s = 0;
    if (ctrl_key_is_set())
        s += sprintf(s, "Ctrl + ");

    if (shift_key_is_set())
        s += sprintf(s, "Shift + ");

    if (alt_key_is_set())
        s += sprintf(s, "Alt + ");

    return mod_key_str;
}

static inline void dispatch_mod_keys(unsigned int modifier_keys_bitmask)
{
    if (modifier_keys_bitmask & ShiftMask)
        set_shift_key();
    if (modifier_keys_bitmask & ControlMask)
        set_ctrl_key();
    if (modifier_keys_bitmask & Mod1Mask) // Mod1 is, usually, the Alt key
        set_alt_key();
}

/* Misc. inline functions */

static inline void move_entity(entity_t *e, float x_offset, float y_offset, float z_offset)
{
    e->position.x += x_offset;
    e->position.y += y_offset;
    e->position.z += z_offset;
}

/* Functions */
int check_for_glx_extension(char *extension, Display *display, int screen_id);
void draw_gl_triangle(void);
int link_gl_functions(void);
unsigned int make_gl_program(char *vertex_shader_source, char *fragment_shader_source);
unsigned int build_shader(char *source, int type);
void test_triangle_logic(player_input_t *input);

#endif
