#ifndef SHINAGE_GAME_H
#define SHINAGE_GAME_H

#include "shinage_common.h"

void draw_gl_pyramid(float *colours, unsigned int program);
void draw_gl_cube(float *colours, unsigned int program);
void draw_static_cubes_scene(game_state_t *g, uint segments);
void draw_bouncing_cube_scene(game_state_t *g);
void log_debug_cpu_computed_vertex_positions(float *vertices, uint count, uint dims);
void test_cube_logic(game_state_t *g, entity_t *e);
void update_global_vars(game_state_t *g);
void draw_fps_counter(game_state_t *g);


GAME_UPDATE(game_update)
{
    update_global_vars(g);
    // left it here
    test_cube_logic(g, &g->test_pyramid);
}

GAME_RENDER(game_render)
{
    update_global_vars(g);

    // re-link against OpenGL so we can use it inside our dynamic lib
    static bool linked = false;
    if (!linked)
        link_gl_functions();

    draw_static_cubes_scene(g, 8);
    draw_fps_counter(g);
}


void draw_gl_pyramid(float *colours, unsigned int program)
{
    float vertices[] = {
       0.0f,   0.43f,   0.0f,
      -0.5f,  -0.43f,  -0.5f,
       0.5f,  -0.43f,  -0.5f,
       0.0f,  -0.43f,   0.5f
    };

    uint num_indices = 12;
    uint indices[] =
      {
        3,1,2, 0,1,2, 0,3,1, 0,2,3
      };

    openGL.glUseProgram(program);

    static int mmatrix_uniform_pos = -1;
    if (mmatrix_uniform_pos == -1)
        mmatrix_uniform_pos = openGL.glGetUniformLocation(program, "modelMatrix");

    static int vmatrix_uniform_pos = -1;
    if (vmatrix_uniform_pos == -1)
        vmatrix_uniform_pos = openGL.glGetUniformLocation(program, "viewMatrix");

    static int pmatrix_uniform_pos = -1;
    if (pmatrix_uniform_pos == -1)
        pmatrix_uniform_pos = openGL.glGetUniformLocation(program, "projMatrix");


    mat4x4f mmatrix = peek(mats->model);
    mat4x4f vmatrix = peek(mats->view);
    mat4x4f pmatrix = peek(mats->projection);

    openGL.glUniformMatrix4fv(mmatrix_uniform_pos, 1, GL_TRUE, mmatrix.v);
    openGL.glUniformMatrix4fv(vmatrix_uniform_pos, 1, GL_TRUE, vmatrix.v);
    openGL.glUniformMatrix4fv(pmatrix_uniform_pos, 1, GL_TRUE, pmatrix.v);

    static unsigned int vao = 0;
    if (!vao)
        openGL.glGenVertexArrays(1, &vao);

    openGL.glBindVertexArray(vao);

    static unsigned int position_bo = 0;
    if (!position_bo)
        openGL.glGenBuffers(1, &position_bo);

    openGL.glBindBuffer(GL_ARRAY_BUFFER, position_bo);
    openGL.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    openGL.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    openGL.glEnableVertexAttribArray(0);

    static unsigned int colour_bo = 0;
    if (!colour_bo)
        openGL.glGenBuffers(1, &colour_bo);

    openGL.glBindBuffer(GL_ARRAY_BUFFER, colour_bo);
    openGL.glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), colours, GL_STATIC_DRAW);
    openGL.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    openGL.glEnableVertexAttribArray(1);

    static unsigned int element_bo = 0;
    if (!element_bo)
        openGL.glGenBuffers(1, &element_bo);

    openGL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_bo);
    openGL.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, (void*)0);
}

bool show_cpu_calculated_matrix;

void draw_gl_cube(float *colours, unsigned int program)
{
    float vertices[] = {
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
    };

    uint num_indices = 36;
    uint indices[] = {
        0,1,3, 1,2,3, 1,5,2, 5,6,2, 4,5,0, 5,1,0,
        3,2,7, 2,6,7, 4,0,7, 0,3,7, 5,4,6, 4,7,6
    };

    openGL.glUseProgram(program);

    static int mmatrix_uniform_pos = -1;
    if (mmatrix_uniform_pos == -1)
        mmatrix_uniform_pos = openGL.glGetUniformLocation(program, "modelMatrix");

    static int vmatrix_uniform_pos = -1;
    if (vmatrix_uniform_pos == -1)
        vmatrix_uniform_pos = openGL.glGetUniformLocation(program, "viewMatrix");

    static int pmatrix_uniform_pos = -1;
    if (pmatrix_uniform_pos == -1)
        pmatrix_uniform_pos = openGL.glGetUniformLocation(program, "projMatrix");


    mat4x4f mmatrix = peek(mats->model);
    mat4x4f vmatrix = peek(mats->view);
    mat4x4f pmatrix = peek(mats->projection);

    openGL.glUniformMatrix4fv(mmatrix_uniform_pos, 1, GL_TRUE, mmatrix.v);
    openGL.glUniformMatrix4fv(vmatrix_uniform_pos, 1, GL_TRUE, vmatrix.v);
    openGL.glUniformMatrix4fv(pmatrix_uniform_pos, 1, GL_TRUE, pmatrix.v);

    static unsigned int vao = 0;
    if (!vao)
        openGL.glGenVertexArrays(1, &vao);

    openGL.glBindVertexArray(vao);

    static unsigned int position_bo = 0;
    if (!position_bo)
        openGL.glGenBuffers(1, &position_bo);

    openGL.glBindBuffer(GL_ARRAY_BUFFER, position_bo);
    openGL.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    openGL.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    openGL.glEnableVertexAttribArray(0);

    static unsigned int colour_bo = 0;
    if (!colour_bo)
        openGL.glGenBuffers(1, &colour_bo);

    openGL.glBindBuffer(GL_ARRAY_BUFFER, colour_bo);
    openGL.glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), colours, GL_STATIC_DRAW);
    openGL.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    openGL.glEnableVertexAttribArray(1);

    static unsigned int element_bo = 0;
    if (!element_bo)
        openGL.glGenBuffers(1, &element_bo);

    openGL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_bo);
    openGL.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, (void*)0);

    if (show_cpu_calculated_matrix)
    {
        log_debug_cpu_computed_vertex_positions(vertices, 8, 3);
        show_cpu_calculated_matrix = false;
    }
}

void draw_static_cubes_scene(game_state_t *g, uint segments)
{
    float colours[9][24] = {
        {0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0},  // Black
        {0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1},  // Blue
        {0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0},  // Green
        {0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1},  // Cyan
        {1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0},  // Red
        {1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 1,0,1},  // Magenta
        {1,1,0, 1,1,0, 1,1,0, 1,1,0, 1,1,0, 1,1,0, 1,1,0, 1,1,0},  // Yellow
        {1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1},  // White
        {0,0,0, 0,0,1, 0,1,0, 0,1,1, 1,0,0, 1,0,1, 1,1,0, 1,1,1}   // Rainbow
    };

    set_mat(MODEL, g);
    push_matrix();
    // The center of the scen will be (0 , 0, -1)
    vec3f trans_the_origin = { .x = 0, .y = 0, .z = -1 };
    translate_matrix(trans_the_origin);
    vec3f scale = { .x = 0.1f, .y = 0.1f, .z = 0.1f };
    scale_matrix(scale);
    draw_gl_pyramid(colours[8], g->simple_color_program); // The center
    scale.x = 10; scale.y = 10; scale.z = 10;
    scale_matrix(scale);

    axis3f_t rot_axis = {
        .vec = { .x = 0, .y = 0, .z = 1 },
        .pnt = { .x = 0, .y = 0, .z = -1 }
    };
    float rot_angle = 2 * M_PI / segments;
    vec3f trans_from_origin = { .x = 0, .y = 2, .z = 0 };
    for (uint i = 0; i < segments; i++)
    {
        push_matrix();
        translate_matrix(trans_from_origin);
        vec3f scale = { .x = 0.3f, .y = 0.3f, .z = 0.3f };
        scale_matrix(scale);
        draw_gl_cube(colours[1], g->simple_color_program);
        pop_matrix();
        rotate_matrix(rot_axis, rot_angle);
    }

    rot_axis.vec.y = 1; rot_axis.vec.z = 0;
    trans_from_origin.y = 0; trans_from_origin.x = 2.5f;
    for (uint i = 0; i < segments; i++)
    {
        push_matrix();
        translate_matrix(trans_from_origin);
        vec3f scale = { .x = 0.3f, .y = 0.3f, .z = 0.3f };
        scale_matrix(scale);
        draw_gl_cube(colours[2], g->simple_color_program);
        pop_matrix();
        rotate_matrix(rot_axis, rot_angle);
    }

    rot_axis.vec.x = 1; rot_axis.vec.y = 0;
    trans_from_origin.x = 0; trans_from_origin.z = 3.0f;
    for (uint i = 0; i < segments; i++)
    {
        push_matrix();
        translate_matrix(trans_from_origin);
        vec3f scale = { .x = 0.3f, .y = 0.3f, .z = 0.3f };
        scale_matrix(scale);
        draw_gl_cube(colours[4], g->simple_color_program);
        pop_matrix();
        rotate_matrix(rot_axis, rot_angle);
    }
    pop_matrix();
}

void draw_bouncing_cube_scene(game_state_t *g)
{
    float colours[] =
        {
            0,   0,   0,  // Black
            0,   0,   1,  // Blue
            0,   1,   0,  // Green
            0,   1,   1,  // Cyan
            1,   0,   0,  // Red
            1,   0,   1,  // Magenta
            1,   1,   0,  // Yellow
            1,   1,   1   // White
        };
    // Dynamic values for a cool animation
    static float scale_fact = 0.5f;
    static float scale_delta = -0.0025f;
    scale_fact += scale_delta;
    if (scale_fact < 0.25f || scale_fact > 0.75f)
        scale_delta = -scale_delta;

    static float x_pos = 0.0f;
    static float x_pos_delta = -0.0005f;
    static float y_pos = 0.0f;
    static float y_pos_delta = -0.0005f;
    static float z_pos = 0.75f;
    static float z_pos_delta = -0.0005f;

    // x_pos += x_pos_delta;
    if (x_pos < -0.25f || x_pos > 0.25f)
        x_pos_delta = -x_pos_delta;
    // y_pos += y_pos_delta;

    if (y_pos < -0.25f || y_pos > 0.25f)
        y_pos_delta = -y_pos_delta;

    z_pos += z_pos_delta;
    if (z_pos < 0.5f || z_pos > 1.0f)
        z_pos_delta = -z_pos_delta;

    static float rot_fact = 0.75f;
    static float rot_delta = -0.025f;
    rot_fact += rot_delta;
    if (rot_fact > M_PI * 2)
        rot_fact = 0;

    set_mat(MODEL, g);
    /* Pushing the matrix makes a copy of the current matrix and stacks it over. The matrix on top will be the
       one to be used */
    push_matrix();
    vec3f scale = { .x = scale_fact, .y = scale_fact, .z = scale_fact };
    scale_matrix(scale);
    vec3f translation = { .x = x_pos, .y = y_pos, .z = z_pos };
    translate_matrix(translation);
    axis3f_t rot_axis =
        {
            .vec = { .x = 1.0f, .y = 0.75f, .z = 1.0f },
            .pnt = { .x = 0, .y = 0, .z = 0 }
        };
    rotate_matrix(rot_axis, rot_fact);
    draw_gl_cube(colours, g->simple_color_program);

    // Stacking another matrix (copies the previous transformations)
    push_matrix();
    scale_matrix(scale);
    translation.x = 0; translation.y = 2; translation.z = 0;
    translate_matrix(translation);
    rotate_matrix(rot_axis, rot_fact);
    draw_gl_cube(colours, g->simple_color_program);
    // Popping the matrix removes the top matrix from the stack
    pop_matrix();
    // The last few transformations on the MODEL matrix have been "undone"

    // And with this, the same code generates the cube on the opposite position of the big one
    rot_axis.vec.x = 0; rot_axis.vec.y = 0; rot_axis.vec.z = 1;
    rotate_matrix(rot_axis, M_PI);

    push_matrix();
    scale_matrix(scale);
    translation.x = 0; translation.y = 2; translation.z = 0;
    translate_matrix(translation);
    rotate_matrix(rot_axis, rot_fact);
    draw_gl_cube(colours, g->simple_color_program);
    pop_matrix();

    pop_matrix();
}

void log_debug_cpu_computed_vertex_positions(float *vertices, uint count, uint dims)
{
    mat4x4f mmatrix = peek(mats->model);
    mat4x4f vmatrix = peek(mats->view);
    mat4x4f pmatrix = peek(mats->projection);
    uint i, j, k;
    vec4f vs_ini[count];
    vec4f vs_mod[count];
    vec4f vs_vis[count];
    vec4f vs_pro[count];
    vec4f vs_div[count];
    for (i = 0; i < count; i++)
    {
        vec4f v = zero_vec4f;
        // In GLSL the default values of a partially declared vec4 are (X, 0, 0, 1)
        for (j = 0; j < dims; j++)
            v.v[j] = vertices[i * dims + j];
        for (k = j; k < 3; k++)
            v.v[k] = 0.0f;
        if (dims < 4)
            v.v[k] = 1;
        vs_ini[i] = v;
        v = mat4x4f_vec4f_prod(mmatrix, v);
        vs_mod[i] = v;
        v = mat4x4f_vec4f_prod(vmatrix, v);
        vs_vis[i] = v;
        v = mat4x4f_vec4f_prod(pmatrix, v);
        vs_pro[i] = v;
        for (j = 0; j < 4; j++)
        {
            v.v[j] = v.v[j] / v.w;
        }
        vs_div[i] = v;
    }
    log_debug_vec4f(vs_ini, count, "OBJECT SPACE");
    log_debug_vec4f(vs_mod, count, "WORLD SPACE");
    log_debug_vec4f(vs_vis, count, "CAMERA SPACE");
    log_debug_vec4f(vs_pro, count, "SCREEN SPACE (NOT NORMALIZED)");
    log_debug_vec4f(vs_div, count, "SCREEN SPACE (PERSPECTIVE DIVISION)");
}

void test_cube_logic(game_state_t *g, entity_t *e)
{
    player_input_t *input = &g->curr_frame_input[PLAYER_1];

    bool right   = is_pressed(input->right);
    bool left    = is_pressed(input->left);
    bool forward = is_pressed(input->forward);
    bool back    = is_pressed(input->back);
    bool up      = is_pressed(input->up);
    bool down    = is_pressed(input->down);
    bool shoulder_left = is_pressed(input->shoulder_left);
    bool shoulder_right = is_pressed(input->shoulder_right);
    bool f1 = is_just_pressed(input->f1);
    bool f2 = is_just_pressed(input->f2);
    int  mouse_x = input->cursor_x_delta;
    int  mouse_y = input->cursor_y_delta;
    bool left_click   = is_just_pressed(input->mouse_left_click);
    bool right_click   = is_just_pressed(input->mouse_right_click);
    bool space = is_just_pressed(input->space);
    float rps = M_PI / 2;
    float angle = rps * get_delta_time(global_clock);
    float mouse_sensitivity = 1/(rps*40.0f);
    float move_sensitivity = 1/20.0f;
    static bool lock_roll = false;

    log_debug("DT %f", dt);

    if (mouse_x)
    {
        set_mat(VIEW, g);
        if (lock_roll)
        {
            add_yaw_world_axis(mouse_sensitivity * mouse_x * dt);
        }
        else
        {
            add_yaw(mouse_sensitivity * mouse_x * dt);
        }
        //log_debug("Added roll of %f", angle);
    }
    if (mouse_y)
    {
        set_mat(VIEW, g);
        add_pitch(mouse_sensitivity * mouse_y * dt);
        //log_debug("Added roll of %f", -angle);
    }
    if (forward || back || up || down || right || left)
    {
        set_mat(VIEW, g);
        move_camera((right - left)*move_sensitivity*dt,(up - down)*move_sensitivity*dt,(forward - back)*move_sensitivity*dt);
        //log_debug("Added pitch of %f", angle);
    }
    if (shoulder_left || shoulder_right)
    {
        set_mat(VIEW, g);
        add_roll((shoulder_right - shoulder_left) * angle * dt);
    }
    if (right_click)
    {
        show_cpu_calculated_matrix = true;
        /* Show the camera position */
    }
    if (left_click)
    {
        build_matrices();
        set_mat(PROJECTION, g);
        float ar = (float)g->window_width / (float)g->window_height;
        perspective_camera(M_PI / 4, ar, 0.1f, 100.0f);
        set_mat(VIEW, g);
        vec3f eye = { .x = 0, .y = 0, .z = -1 };
        vec3f poi = { .x = 0, .y = 0, .z = 0 };
        look_at(eye, poi, up_vector);
        log_debug("Reset in frame  %d", g->framecount);
    }

    if (space)
    {
        set_mat(VIEW, g);
        vec3f camera_position = get_position();
        log_debug("Camera position %f %f %f", camera_position.x, camera_position.y, camera_position.z);
    }

    if (f1)
    {
        int new_state = (input->pointer_state == NORMAL ? GRABBED : NORMAL);
        g->curr_frame_input[PLAYER_1].pointer_state = new_state;
    }
    if (f2)
    {
        lock_roll = !lock_roll;
    }

    if (false) // TODO: Placeholder to please the compiler
        log_debug("Ye [%f]", e->position.x);
}

void draw_fps_counter(game_state_t *g)
{
    static char str[32] = "0 FPS (0 ms)";
    static double total = 0.0;
    const int freq = 30;
    total += dt;
    /* Only recalculate every few frames to avoid excessive flickering */
    if (g->framecount && !(g->framecount % freq))
    {
        sprintf(str, "%.2f FPS (%.2f ms)", (1.0*freq/total), total*1000.0/freq);
        total = 0.0;
    }
    vec3f font_color = { .x = 1.0f, .y = 1.0f, .z = 1.0f };
    render_text(str, 5.0f, g->window_height - 20.0f, g->window_width, g->window_height, 0.5f, font_color);
}

#endif
