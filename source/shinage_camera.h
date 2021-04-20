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
    mat4x4f vmatrix = identity_matrix_4x4;
    // The camera is brought to the origin
    mat4x4f translation_matrix =
    {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  e.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,   .d2 =  e.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,   .d3 =  e.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };
    vmatrix = mat4x4f_prod(vmatrix, translation_matrix);

    // The LOOK vector is rotated so it overlaps with the Z axis
    vec3f look = normalize3f(diff3f(poi, e));
    // The referencial axis is rotated to match one of the cartesian axes (Z in this case)
    // These are projections in their respectives planes
    vec3f xy_look_aux = { .x = look.x, .y = look.y, .z = 0 };
    vec3f xz_look_aux = { .x = look.x, .y = 0, .z = look.z };
    // And these are the angles they form with the vec
    float lon = get_angle3f(xz_look_aux, z_dir_vec3f);
    float lat = get_angle3f(xy_look_aux, x_dir_vec3f);
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
    vmatrix = mat4x4f_prod(vmatrix, rotation_matrix_lon);
    mat4x4f rotation_matrix_lat =
    {
        .a1 = 1.0f,        .b1 = 0.0f,        .c1 = 0.0f,        .d1 =  0.0f,
        .a2 = 0.0f,        .b2 = cos(-lat),  .c2 = -sin(-lat), .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = sin(-lat),  .c3 = cos(-lat),  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,        .c4 = 0.0f,        .d4 =  1.0f
    };
    vmatrix = mat4x4f_prod(vmatrix, rotation_matrix_lat);
   
    // Now the UP vector (perpendicular to the LOOK vector) is rotated so it overlaps with the Y axis
    // To get this UP vector we first cross_product3f the global UP given as a parameter with
    // the LOOK vector, and then, this new (right) vector is cross_product3f with the LOOK vector
    vec3f right = cross_product3f(up, look);
    vec3f local_up = cross_product3f(look, right);
    vec4f up4f = { .x = local_up.x, .y = local_up.y, .z = local_up.z, .w = 0 };
    up4f = mat4x4f_vec4f_prod(rotation_matrix_lat, up4f);
    vec3f xy_up_aux = { .x = up4f.x, .y = up4f.y, .z = 0 };
    // And these are the angles they form with the vec
    float angle = get_angle3f(xy_up_aux, y_dir_vec3f);
    // TODO: Error handling or logging
    if (angle == -1)
    {
        angle = 0.0f;
    }
    mat4x4f rotation_matrix_around_z = {
        .a1 = cos(-angle),   .b1 = -sin(-angle),  .c1 = 0.0f,  .d1 =  0.0f,
        .a2 = sin(-angle),   .b2 = cos(-angle),   .c2 = 0.0f,  .d2 =  0.0f,
        .a3 = 0.0f,          .b3 = 0.0f,          .c3 = 1.0f,  .d3 =  0.0f,
        .a4 = 0.0f,          .b4 = 0.0f,          .c4 = 0.0f,  .d4 =  1.0f
    };
    vmatrix = mat4x4f_prod(vmatrix, rotation_matrix_around_z);

    push(mats.view, vmatrix);
}

/*
*   Sets the camera projection as perspective
*   Modifies the PROJECTION matrix
*/
void perspective_camera(float fov_y, float ar, float n, float f)
{
    float tan_fov = tan( fov_y * 0.5 );
    float z_range = n - f;

    mat4x4f pmatrix =
    {
        .a1 = 1.0 / (tan_fov * ar),  .b1 = 0.0f,           .c1 = 0.0f,                .d1 = 0.0f,
        .a2 = 0.0f,                  .b2 = 1.0 / tan_fov,  .c2 = 0.0f,                .d2 = 0.0f,
        .a3 = 0.0f,                  .b3 = 0.0f,           .c3 = (f + n) / z_range,   .d3 = 2*f*n / z_range,
        .a4 = 0.0f,                  .b4 = 0.0f,           .c4 = -1.0,                .d4 = 0.0f
    };
    push(mats.projection, pmatrix);
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


#endif
