#ifndef SHINAGE_COMMON_H
#define SHINAGE_COMMON_H

/* Cross-platform lib/CRT includes */
#include <time.h>

/* Internal includes */
#include "shinage_ints.h"
#include "shinage_debug.h"
#include "shinage_math.h"
#include "shinage_matrix_stack_ops.h"
#include "shinage_input.h"
#include "shinage_opengl_signatures.h"
#include "shinage_shaders.h"
#include "shinage_scene.h"
#include "shinage_utils.h"

/* shinage_text also includes ft2build.h and FT_FREETYPE_H */
#ifdef __linux__
#include "x11_shinage_text.h"
#else
// TODO: Add Windows-specific text rendering
#error "Windows code WIP"
#endif

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

char *version = "0.2";

/* Timing globals */
double target_s_per_frame = (1.0/60.0); // ~60 fps
double dt; // time diff between this and last frame
/* We start up the program with the assumption that Vsync is ON.
   TODO: Check if this is a good assumption on all X11 WMs */
bool vsync = true;

typedef struct {
    // State info
    main_loop_state_t loop_state;
    entity_t *entities; // ideally we would use these
    entity_t test_triangle;
    entity_t test_pyramid;
    model_t sun;
    camera_t main_camera;

    player_input_t *curr_frame_input;
    player_input_t *last_frame_input;

    // Drawing info
    gl_matrices_t mats;
    matrix_stack_t *active_mat;
    uint simple_color_program;
    uint single_light_program;
    character_t *default_charmap;

    // Window info
    int window_width;
    int window_height;
    bool vsync;

    // Timing info
    int framecount;
    double target_s_per_frame;
    double game_clock;
    double dt;
} game_state_t;

// Typedef definitions for code injection
#define GAME_UPDATE(funcname) void funcname(game_state_t *g)
typedef GAME_UPDATE(game_update_f);

#define GAME_RENDER(funcname) void funcname(game_state_t *g)
typedef GAME_RENDER(game_render_f);

typedef struct {
    game_update_f *game_update;
    game_render_f *game_render;
} game_code_t;

// Convenience global vars
matrix_stack_t *active_mat = NULL;
gl_matrices_t *mats = NULL;
double *global_clock = NULL;
double dt = 0;

/* Misc. inline functions */

static inline void update_global_vars(game_state_t *g)
{
    active_mat = g->active_mat;
    mats = &g->mats;
    global_clock = &g->game_clock;
    dt = g->dt;
}

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

static inline float get_delta_time(double *clock)
{
    double time_lf = *clock;
    if (!time_lf)
        time_lf = get_current_time();

    double time_cf = get_current_time();
    float delta = time_cf - time_lf;

    *clock = time_cf;
    return delta;
}

static inline void set_mat(matrix_t m, game_state_t *g)
{
    switch (m)
    {
    case MODEL:
        g->active_mat = g->mats.model;
        break;
    case VIEW:
        g->active_mat = g->mats.view;
        break;
    case PROJECTION:
        g->active_mat = g->mats.projection;
        break;
    default:
        break;
    }
    // HACK: set_mat takes params now, but the other mat functions keep using global state
    update_global_vars(g);
}

#endif
