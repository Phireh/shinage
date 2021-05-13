#ifndef SHINAGE_MATRIX_STACK_OPS_H
#define SHINAGE_MATRIX_STACK_OPS_H

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

/*
 *   Sets the target for the camera.
 *   NOTE: The up vector is the subjective vertical. Rotation around the w vector. It has to be perpendicular to the look vector
 *   Modifies the VIEW matrix.
 */
mat4x4f get_look_at_mat4x4f(vec3f e, vec3f poi, vec3f up)
{
    vec3f f = normalize3f(diff3f(poi, e));
    vec3f u = normalize3f(up);
    vec3f s = normalize3f(cross_product3f(f, u));
    u = cross_product3f(s, f);
    mat4x4f mat = {
        .a1 =  s.x,  .b1 =  s.y,  .c1 =  s.z,  .d1 = -dot_product3f(s, e),
        .a2 =  u.x,  .b2 =  u.y,  .c2 =  u.z,  .d2 = -dot_product3f(u, e),
        .a3 = -f.x,  .b3 = -f.y,  .c3 = -f.z,  .d3 =  dot_product3f(f, e),
        .a4 =  0.0f, .b4 =  0.0f, .c4 =  0.0f, .d4 = 1.0f
    };
    return mat;
}

void look_at(vec3f e, vec3f poi, vec3f up)
{
    if (!active_mat)
        return;

    pop(active_mat);
    mat4x4f aux_mat = get_look_at_mat4x4f(e, poi, up);
    push(active_mat, aux_mat);
}

/*
 *   Sets the camera projection as perspective
 *   Modifies the PROJECTION matrix
 */
mat4x4f get_perspective_camera_mat4x4f(float fov_y, float ar, float n, float f)
{
    float tan_fov = tan( fov_y * 0.5 );
    float z_range = n - f;

    mat4x4f mat = {
        .a1 = 1.0 / (tan_fov * ar),  .b1 = 0.0f,           .c1 = 0.0f,                .d1 = 0.0f,
        .a2 = 0.0f,                  .b2 = 1.0 / tan_fov,  .c2 = 0.0f,                .d2 = 0.0f,
        .a3 = 0.0f,                  .b3 = 0.0f,           .c3 = (f + n) / z_range,   .d3 = 2*f*n / z_range,
        .a4 = 0.0f,                  .b4 = 0.0f,           .c4 = -1.0,                .d4 = 0.0f
    };
    return mat;
}

void perspective_camera(float fov_y, float ar, float n, float f)
{
	if (!active_mat)
        return;

    pop(active_mat);
    mat4x4f aux_mat = get_perspective_camera_mat4x4f(fov_y, ar, n, f);
    push(active_mat, aux_mat);
}

/* NOTE: In our representation the camera lookAt vector has opposite sign
   to what we see. This means that, for camera transforms, the Z coordinate
   should be multiplied by -1.

   This function is meant to be more general, so the callee is responsible
   for remembering that. A convenience function is provided below.
 */
mat4x4f get_translated_matrix_mat4x4f(mat4x4f mat, vec3f desp)
{
    
    //log_debug_matx4f(&aux, "MODEL BEFORE TRANSLATION:");
    mat4x4f translate_matrix = {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  desp.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,   .d2 =  desp.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,   .d3 =  desp.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };
    //aux = mat4x4f_prod(translate_matrix,  aux);
    mat = mat4x4f_prod(mat, translate_matrix);
    //log_debug_matx4f(&aux, "MODEL AFTER TRANSLATION:");
    return mat;
}

void translate_matrix(vec3f desp)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
    aux = get_translated_matrix_mat4x4f(aux, desp);
    push(active_mat, aux);
}

mat4x4f get_scaled_matrix_mat4x4f(mat4x4f mat, vec3f sc)
{
    mat4x4f scale_matrix = {
        .a1 = sc.x,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  0.0f,
        .a2 = 0.0f,  .b2 = sc.y,  .c2 = 0.0f,   .d2 =  0.0f,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = sc.z,   .d3 =  0.0f,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };
    mat = mat4x4f_prod(mat, scale_matrix);
    return mat;
}

void scale_matrix(vec3f sc)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
    aux = get_scaled_matrix_mat4x4f(aux, sc);
    push(active_mat, aux);
}

mat4x4f get_rotated_matrix_mat4x4f(mat4x4f mat, axis3f_t rot_axis, float angle)
{
    vec3f axis_pnt = rot_axis.pnt;
    vec3f axis_vec = rot_axis.vec;

    // If there is not a vector for reference, there is no rotation
    if (!length3f(axis_vec))
        return identity_matrix_4x4;

    // The axis has to be translated to the origin
    mat4x4f translation_matrix = {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  -axis_pnt.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,   .d2 =  -axis_pnt.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,   .d3 =  -axis_pnt.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };
    mat = mat4x4f_prod(mat, translation_matrix);
    // This matrix will be used to unmake the translation
    mat4x4f un_translation_matrix = {
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

    mat4x4f rotation_matrix_lon = {
        .a1 = cos(-lon),    .b1 = 0.0f,      .c1 = sin(-lon),    .d1 =  0.0f,
        .a2 = 0.0f,         .b2 = 1.0f,      .c2 = 0.0f,         .d2 =  0.0f,
        .a3 = -sin(-lon),   .b3 = 0.0f,      .c3 = cos(-lon),    .d3 =  0.0f,
        .a4 = 0.0f,         .b4 = 0.0f,      .c4 = 0.0f,         .d4 =  1.0f
    };
    mat = mat4x4f_prod(mat, rotation_matrix_lon);
    mat4x4f un_rotation_matrix_lon = {
        .a1 = cos(lon),    .b1 = 0.0f,      .c1 = sin(lon),    .d1 =  0.0f,
        .a2 = 0.0f,        .b2 = 1.0f,      .c2 = 0.0f,        .d2 =  0.0f,
        .a3 = -sin(lon),   .b3 = 0.0f,      .c3 = cos(lon),    .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,      .c4 = 0.0f,        .d4 =  1.0f
    };
    // These matrices will be used to unmake the auxiliar rotations
    mat4x4f rotation_matrix_lat = {
        .a1 = 1.0f,        .b1 = 0.0f,       .c1 = 0.0f,       .d1 =  0.0f,
        .a2 = 0.0f,        .b2 = cos(-lat),  .c2 = -sin(-lat), .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = sin(-lat),  .c3 = cos(-lat),  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,       .c4 = 0.0f,       .d4 =  1.0f
    };
    mat = mat4x4f_prod(mat, rotation_matrix_lat);
    mat4x4f un_rotation_matrix_lat = {
        .a1 = 1.0f,        .b1 = 0.0f,      .c1 = 0.0f,      .d1 =  0.0f,
        .a2 = 0.0f,        .b2 = cos(lat),  .c2 = -sin(lat), .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = sin(lat),  .c3 = cos(lat),  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,      .c4 = 0.0f,      .d4 =  1.0f
    };
    // With the new rotations the axis will be overlapping the X axis, so the rotation
    // will be performed around it. This is the transformation that won't be undone
    mat4x4f rotation_matrix_around_z = {
        .a1 = cos(angle),  .b1 = -sin(angle),  .c1 = 0.0f,  .d1 =  0.0f,
        .a2 = sin(angle),  .b2 = cos(angle) ,  .c2 = 0.0f,  .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = 0.0f,         .c3 = 1.0f,  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,         .c4 = 0.0f,  .d4 =  1.0f
    };
    mat = mat4x4f_prod(mat, rotation_matrix_around_z);

    // The auxiliar transformations are unmade in reverse order
    mat = mat4x4f_prod(mat, un_rotation_matrix_lat);
    mat = mat4x4f_prod(mat, un_rotation_matrix_lon);
    mat = mat4x4f_prod(mat, un_translation_matrix);

    return mat;
}

void rotate_matrix(axis3f_t rot_axis, float angle)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
    aux = get_rotated_matrix_mat4x4f(aux, rot_axis, angle);
    push(active_mat, aux);
}

static inline vec3f get_position_mat4x4f(mat4x4f mat)
{
    mat4x4f inv_mat = inverse_mat4x4f(mat, false, false);
    vec3f pos = { .x = inv_mat.d1, .y = inv_mat.d2, .z = inv_mat.d3 };
    return pos;
}

static inline vec3f get_position()
{
    if (!active_mat)
        return nan_vec3f;
    mat4x4f aux = peek(active_mat);
	return get_position_mat4x4f(aux);
}

mat4x4f get_rotated_self_mat4x4f(mat4x4f mat, vec3f axis, float angle)
{
    //vec3f pos = get_position();
    vec3f pos = { .x = 0, .y = 0, .z = 0 };
    vec4f axis_ported = { .x = axis.x, .y = axis.y, .z = axis.z, .w = 0 };
    axis_ported = mat4x4f_vec4f_prod(mat, axis_ported);
    axis.x = axis_ported.x; axis.y = axis_ported.y; axis.z = axis_ported.z;

    log_debug("Ported vec.: (%f, %f, %f)", axis.x, axis.y, axis.z);
    log_debug("Pos. from matrix: (%f, %f, %f)", pos.x, pos.y, pos.z);
    axis3f_t rot_axis = {
        .vec = axis,
        .pnt = pos
    };
    return get_rotated_matrix_mat4x4f(mat, rot_axis, angle);
}

void rotate_self(vec3f axis, float angle)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
	aux = get_rotated_self_mat4x4f(aux, axis, angle);
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

bool pop_matrix()
{
	if (!active_mat || is_empty(active_mat))
        return false;

	pop(active_mat);
    return true;
}

mat4x4f get_added_pitch_mat4x4f(mat4x4f mat, float angle)
{
    mat4x4f rotation_matrix_around_x = {
        .a1 = 1.0f,        .b1 = 0.0f,        .c1 = 0.0f,        .d1 =  0.0f,
        .a2 = 0.0f,        .b2 = cos(angle),  .c2 = -sin(angle), .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = sin(angle),  .c3 = cos(angle),  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,        .c4 = 0.0f,        .d4 =  1.0f
    };
    /* NOTE: The multiplication order is important. The transformations to the world happen
       in left-to-right order as we multiply matrices. We want to apply this rotation *first*,
       before we translate the camera.

       The left-to-right order is because we use row-first matrices. In OpenGL we'd
       have to reverse the order of multiplication.
    */
    mat = mat4x4f_prod(rotation_matrix_around_x, mat);
    return mat;
}

void add_pitch(float angle)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
	aux = get_added_pitch_mat4x4f(aux, angle);
    push(active_mat, aux);
}

mat4x4f get_added_yaw_mat4x4f(mat4x4f mat, float angle)
{
    mat4x4f rotation_matrix_around_y = {
        .a1 = cos(angle),   .b1 = 0.0f,   .c1 = sin(angle),   .d1 =  0.0f,
        .a2 = 0.0f,         .b2 = 1.0f,   .c2 = 0.0f,         .d2 =  0.0f,
        .a3 = -sin(angle),  .b3 = 0.0f,   .c3 = cos(angle),   .d3 =  0.0f,
        .a4 = 0.0f,         .b4 = 0.0f,   .c4 = 0.0f,         .d4 =  1.0f
    };
    /* NOTE: The multiplication order is important. The transformations to the world happen
       in left-to-right order as we multiply matrices. We want to apply this rotation *first*,
       before we translate the camera.

       The left-to-right order is because we use row-first matrices. In OpenGL we'd
       have to reverse the order of multiplication.
    */
    mat = mat4x4f_prod(rotation_matrix_around_y, mat);
    return mat;
}

void add_yaw(float angle)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
    aux = get_added_yaw_mat4x4f(aux, angle);
    push(active_mat, aux);
}

mat4x4f get_added_yaw_world_axis_mat4x4f(mat4x4f mat, float angle)
{
    vec3f camera_pos = get_position_mat4x4f(mat);

    mat4x4f rotation_matrix_around_y = {
        .a1 = cos(angle),   .b1 = 0.0f,   .c1 = sin(angle),   .d1 =  0.0f,
        .a2 = 0.0f,         .b2 = 1.0f,   .c2 = 0.0f,         .d2 =  0.0f,
        .a3 = -sin(angle),  .b3 = 0.0f,   .c3 = cos(angle),   .d3 =  0.0f,
        .a4 = 0.0f,         .b4 = 0.0f,   .c4 = 0.0f,         .d4 =  1.0f
    };

    /* Translate the camera to 0,0,0 so we can rotate around the world Y axis */
    mat4x4f translate_to_origin_mat = {
        .a1 = 1, .b1 = 0, .c1 = 0, .d1 = camera_pos.x,
        .a2 = 0, .b2 = 1, .c2 = 0, .d2 = camera_pos.y,
        .a3 = 0, .b3 = 0, .c3 = 1, .d3 = camera_pos.z,
        .a4 = 0, .b4 = 0, .c4 = 0, .d4 = 1
    };

    mat4x4f return_from_origin_mat = {
        .a1 = 1, .b1 = 0, .c1 = 0, .d1 = -camera_pos.x,
        .a2 = 0, .b2 = 1, .c2 = 0, .d2 = -camera_pos.y,
        .a3 = 0, .b3 = 0, .c3 = 1, .d3 = -camera_pos.z,
        .a4 = 0, .b4 = 0, .c4 = 0, .d4 = 1
    };


    /* NOTE: The multiplication order is important. The transformations to the world happen
       in left-to-right order as we multiply matrices. We want to take the View matrix
       already created and apply three new transformations to it:

       NewMat = ViewMat * ToOrigin * Rotate * BackFromOrigin

       The left-to-right order is because we use row-first matrices. In OpenGL we'd
       have to reverse the order of multiplication.
*/

    mat = mat4x4f_prod(mat, translate_to_origin_mat);
    mat = mat4x4f_prod(mat, rotation_matrix_around_y);
    mat = mat4x4f_prod(mat, return_from_origin_mat);
    return mat;
}

void add_yaw_world_axis(float angle)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
	aux = get_added_yaw_world_axis_mat4x4f(aux, angle);
    push(active_mat, aux);
}

mat4x4f get_added_roll_mat4x4f(mat4x4f mat, float angle)
{
    mat4x4f rotation_matrix_around_z = {
        .a1 = cos(angle),  .b1 = -sin(angle),  .c1 = 0.0f,  .d1 =  0.0f,
        .a2 = sin(angle),  .b2 = cos(angle) ,  .c2 = 0.0f,  .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = 0.0f,         .c3 = 1.0f,  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,         .c4 = 0.0f,  .d4 =  1.0f
    };
    /* NOTE: The multiplication order is important. The transformations to the world happen
       in left-to-right order as we multiply matrices. We want to apply this rotation *first*,
       before we translate the camera.

       The left-to-right order is because we use row-first matrices. In OpenGL we'd
       have to reverse the order of multiplication.
    */
    mat = mat4x4f_prod(rotation_matrix_around_z, mat);
    return mat;
}

void add_roll(float angle)
{
    if (!active_mat)
        return;

    mat4x4f aux = pop(active_mat);
	aux = get_added_roll_mat4x4f(aux, angle);
    push(active_mat, aux);
}

void move_camera(float x, float y, float z)
{
    if (!active_mat)
        return;

    mat4x4f mat = pop(active_mat);
    vec3f desp = { .x = x, .y = y, .z = -z };
    mat = get_translated_matrix_mat4x4f(mat, desp);
    push(active_mat, mat);
}

#endif