#ifndef SHINAGE_CAMERA_H
#define SHINAGE_CAMERA_H
#include <stdbool.h>

#include "shinage_math.h"
#include "shinage_debug.h"
#include "shinage_stack_structures.h"

typedef enum { MODEL, VIEW, PROJECTION } matrix_t;

typedef struct
{
    matrix_stack_t *model;
    matrix_stack_t *view;
    matrix_stack_t *projection; 
} gl_matrices_t;

gl_matrices_t mats;
matrix_stack_t *active_mat;

void build_matrices()
{
    mats.model = build_stack(10);
    push(mats.model, identity_matrix_4x4);
    mats.view = build_stack(10);
    push(mats.view, identity_matrix_4x4);
    mats.projection = build_stack(10);
    push(mats.projection, identity_matrix_4x4);
}

/*
*   Sets the target for the camera.
*   NOTE: The up vector is the subjective vertical. Rotation around the w vector. It has to be perpendicular to the look vector
*   Modifies the VIEW matrix.
*/
void look_at(vec3f e, vec3f poi, vec3f up)
{
    if (!active_mat)
        return;

    pop(active_mat);

    // The w is the vector with the same direction of the look vector, but contrary sense
    vec3f w = normalize3f(diff3f(e, poi));
    // The u vector is perpendicular to the plane formed by the up and the w vectors
    vec3f u = normalize3f(cross_product3f(w, up)); // The global up vector should be unitary, but just in case, we normallyze
    // And the vector v is perpendicular to the plane formed by the w and u vectors
    vec3f v = cross_product3f(u, w); // Both are already unitary vectors, no need to normallize
    // Rotation to make the bases coincide (I guess xD)
    mat4x4f mr = {
        .a1 = u.x,  .b1 = u.y,  .c1 = u.z,  .d1 = 0.0f,
        .a2 = v.x,  .b2 = v.y,  .c2 = v.z,  .d2 = 0.0f,
        .a3 = w.x,  .b3 = w.y,  .c3 = w.z,  .d3 = 0.0f,
        .a4 = 0.0f, .b4 = 0.0f, .c4 = 0.0f, .d4 = 1.0f
    };
    // Translation to make the origins coincide (I guess xD)
    mat4x4f mt = {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,  .d1 = -e.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,  .d2 = -e.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,  .d3 = -e.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,  .d4 = 1.0f
    };
    mat4x4f mat = mat4x4f_prod(mr, mt);
    push(active_mat, mat);
}

/*
*   Sets the camera projection as perspective
*   Modifies the PROJECTION matrix
*/
void perspective_camera(float fov_y, float ar, float n, float f)
{
    if (!active_mat)
        return;

    pop(active_mat);

    float tan_fov = tan( fov_y * 0.5 );
    float z_range = n - f;

    mat4x4f mat =
    {
        .a1 = 1.0 / (tan_fov * ar),  .b1 = 0.0f,           .c1 = 0.0f,                .d1 = 0.0f,
        .a2 = 0.0f,                  .b2 = 1.0 / tan_fov,  .c2 = 0.0f,                .d2 = 0.0f,
        .a3 = 0.0f,                  .b3 = 0.0f,           .c3 = (f + n) / z_range,   .d3 = 2*f*n / z_range,
        .a4 = 0.0f,                  .b4 = 0.0f,           .c4 = -1.0,                .d4 = 0.0f
    };
    push(active_mat, mat);
}

void set_mat(matrix_t mat)
{
    switch (mat)
    {
        case MODEL:
            active_mat = mats.model;
            break;
        case VIEW:
            active_mat = mats.view;
            break;
        case PROJECTION:
            active_mat = mats.projection;
            break;
        default:
            break;
    }
}

void translate_matrix(vec3f desp)
{
    if (!active_mat)
        return;
    
    mat4x4f aux = pop(active_mat);
    //log_debug_matx4f(&aux, "MODEL BEFORE TRANSLATION:");
    mat4x4f translate_matrix =
    {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  desp.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,   .d2 =  desp.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,   .d3 =  desp.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };
    //aux = mat4x4f_prod(translate_matrix,  aux);
    aux = mat4x4f_prod(aux, translate_matrix);
    //log_debug_matx4f(&aux, "MODEL AFTER TRANSLATION:");
    push(active_mat, aux);
}

void scale_matrix(vec3f sc)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
    //log_debug_matx4f(&aux, "MODEL BEFORE SCALETION:");
    mat4x4f scale_matrix =
    {
        .a1 = sc.x,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  0.0f,
        .a2 = 0.0f,  .b2 = sc.y,  .c2 = 0.0f,   .d2 =  0.0f,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = sc.z,   .d3 =  0.0f,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, scale_matrix);
    //log_debug_matx4f(&aux, "MODEL AFTER SCALETION:");
    push(active_mat, aux);
}

void rotate_matrix(axis3f_t rot_axis, float angle)
{
    if (!active_mat)
        return;

    vec3f axis_pnt = rot_axis.pnt;
    vec3f axis_vec = rot_axis.vec;

    // If there is not a vector for reference, there is no rotation
    if (!length3f(axis_vec))
        return;

    mat4x4f aux = pop(active_mat);

    // The axis has to be translated to the origin
    mat4x4f translation_matrix =
    {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  -axis_pnt.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,   .d2 =  -axis_pnt.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,   .d3 =  -axis_pnt.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, translation_matrix);
    // This matrix will be used to unmake the translation
    mat4x4f un_translation_matrix =
    {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  axis_pnt.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,   .d2 =  axis_pnt.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,   .d3 =  axis_pnt.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };

    // The referencial axis is rotated to match one of the cartesian axes (Z in this case)
    // These are projections in their respectives planes
    vec3f xy_aux = { .x = axis_vec.x, .y = axis_vec.y, .z = 0 };
    vec3f xz_aux = { .x = axis_vec.x, .y = 0, .z = axis_vec.z };
    // And these are the angles they form with the vec
    float lon = get_angle3f(xz_aux, z_dir_vec3f);
    float lat = get_angle3f(xy_aux, x_dir_vec3f);

    // TODO: Error handling or logging
    if (lon == -1)
    {
        lon = 0.0f;
    }
    if (lat == -1)
    {
        lat = 0.0f;
    }

    mat4x4f rotation_matrix_lon =
    {
        .a1 = cos(-lon),     .b1 = 0.0f,      .c1 = sin(-lon),     .d1 =  0.0f,
        .a2 = 0.0f,         .b2 = 1.0f,      .c2 = 0.0f,         .d2 =  0.0f,
        .a3 = -sin(-lon),    .b3 = 0.0f,      .c3 = cos(-lon),     .d3 =  0.0f,
        .a4 = 0.0f,         .b4 = 0.0f,      .c4 = 0.0f,         .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, rotation_matrix_lon);
    mat4x4f un_rotation_matrix_lon = 
    {
        .a1 = cos(lon),    .b1 = 0.0f,      .c1 = sin(lon),    .d1 =  0.0f,
        .a2 = 0.0f,         .b2 = 1.0f,      .c2 = 0.0f,         .d2 =  0.0f,
        .a3 = -sin(lon),   .b3 = 0.0f,      .c3 = cos(lon),    .d3 =  0.0f,
        .a4 = 0.0f,         .b4 = 0.0f,      .c4 = 0.0f,         .d4 =  1.0f
    };
    // These matrices will be used to unmake the auxiliar rotations
    mat4x4f rotation_matrix_lat =
    {
        .a1 = 1.0f,        .b1 = 0.0f,        .c1 = 0.0f,        .d1 =  0.0f,
        .a2 = 0.0f,        .b2 = cos(-lat),  .c2 = -sin(-lat), .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = sin(-lat),  .c3 = cos(-lat),  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,        .c4 = 0.0f,        .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, rotation_matrix_lat);
    mat4x4f un_rotation_matrix_lat =
    {
        .a1 = 1.0f,        .b1 = 0.0f,        .c1 = 0.0f,        .d1 =  0.0f,
        .a2 = 0.0f,        .b2 = cos(lat),  .c2 = -sin(lat), .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = sin(lat),  .c3 = cos(lat),  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,        .c4 = 0.0f,        .d4 =  1.0f
    };
    // With the new rotations the axis will be overlapping the X axis, so the rotation
    // will be performed around it. This is the transformation that won't be undone
    mat4x4f rotation_matrix_around_z =
    {
        .a1 = cos(angle),  .b1 = -sin(angle),  .c1 = 0.0f,  .d1 =  0.0f,
        .a2 = sin(angle),  .b2 = cos(angle) ,  .c2 = 0.0f,  .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = 0.0f,         .c3 = 1.0f,  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,         .c4 = 0.0f,  .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, rotation_matrix_around_z);

    // The auxiliar transformations are unmade in reverse order
    aux = mat4x4f_prod(aux, un_rotation_matrix_lat);
    aux = mat4x4f_prod(aux, un_rotation_matrix_lon);
    aux = mat4x4f_prod(aux, un_translation_matrix);

    push(active_mat, aux);
}

void rotate_self(vec3f axis, float angle)
{
     if (!active_mat)
        return;

    mat4x4f mat = peek(active_mat);
    mat4x4f inv_mat = inverse_mat4x4f(mat, true, false);
    log_debug_matx4f(&mat, "VIEW");
    log_debug_matx4f(&inv_mat, "Inverse of VIEW");

    vec3f pos = { .x = inv_mat.d1, .y = inv_mat.d2, .z = inv_mat.d3 };


    axis3f_t rot_axis =
    {
        .vec = axis,
        .pnt = pos
    };
    rotate_matrix(rot_axis, angle);
}

bool push_matrix()
{
    if (!active_mat)
        return false;

    mat4x4f aux = peek(active_mat);
    bool done = push(active_mat, aux);
    return done;
}

void pop_matrix()
{
    if (!active_mat)
        return;

    pop(active_mat);
}

void add_roll(float angle)
{
    vec3f rot_vec = { .x = 1, .y = 0, .z = 0 };
    rotate_self(rot_vec, angle);
}

void add_pitch(float angle)
{
    vec3f rot_vec = { .x = 0, .y = 1, .z = 0 };
    rotate_self(rot_vec, angle);
}

void add_yaw(float angle)
{
    vec3f rot_vec = { .x = 0, .y = 0, .z = 1 };
    rotate_self(rot_vec, angle);
}


#endif
