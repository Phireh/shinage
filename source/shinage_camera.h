#ifndef SHINAGE_CAMERA_H
#define SHINAGE_CAMERA_H
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
*   NOTE: The up vector is the subjective vertical. Rotation arround the w vector. It has to be perpendicular to the look vector
*   Modifies the VIEW matrix.
*/
void look_at(vec3f e, vec3f poi, vec3f up)
{
    vec3f look = normalize3f(diff3f(poi, e));
    // The w is the vector with the same direction of the look vector, but contrary sense
    vec3f w = diff3f(zero_vec3f, look);
    // The u vector is perpendicular to the plane formed by the up and the w vectors
    vec3f u = normalize3f(cross_product3f(up, w));
    // And the vector v is perpendicular to the plane formed by the w and u vectors
    vec3f v = cross_product3f(w, u); // Both are already unitary vectors, no need to normallize
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
    mat4x4f vmatrix = mat4x4f_prod(mt, mr);
    log_debug_matx4f(&vmatrix, "VIEW_MATRIX");
    push(mats.view, vmatrix);
}

/*
*   Sets the camera projection as perspective
*   Modifies the PROJECTION matrix
*/
void perspective_camera(float fovy, float ar, float near, float far)
{
    float D2R = M_PI / 180.0;
    float y_scale = 1.0 / tan(D2R * fovy / 2);
    float x_scale = y_scale / ar;
    float nearmfar = near - far;
    mat4x4f pmatrix =
    {
        .a1 = x_scale,  .b1 = 0.0f,     .c1 = 0.0f,                     .d1 = 0.0f,
        .a2 = 0.0f,     .b2 = y_scale,  .c2 = 0.0f,                     .d2 = 0.0f,
        .a3 = 0.0f,     .b3 = 0.0f,     .c3 = (far + near) / nearmfar,  .d3 = -1.0,
        .a4 = 0.0f,     .b4 = 0.0f,     .c4 = 2*far*near / nearmfar,    .d4 = 0.0f
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

void rotate_matrix(exe3f_t rot_exe, float angle)
{
    if (!active_mat)
        return;

    vec3f exe_pnt = rot_exe.pnt;
    vec3f exe_vec = rot_exe.vec;

    // If there is not a vector for reference, there is no rotation
    if (!length3f(exe_vec))
        return;

    mat4x4f aux = pop(active_mat);

    // The exe has to be translated to the origin
    mat4x4f translation_matrix =
    {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  -exe_pnt.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,   .d2 =  -exe_pnt.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,   .d3 =  -exe_pnt.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, translation_matrix);
    // This matrix will be used to unmake the translation
    mat4x4f un_translation_matrix =
    {
        .a1 = 1.0f,  .b1 = 0.0f,  .c1 = 0.0f,   .d1 =  exe_pnt.x,
        .a2 = 0.0f,  .b2 = 1.0f,  .c2 = 0.0f,   .d2 =  exe_pnt.y,
        .a3 = 0.0f,  .b3 = 0.0f,  .c3 = 1.0f,   .d3 =  exe_pnt.z,
        .a4 = 0.0f,  .b4 = 0.0f,  .c4 = 0.0f,   .d4 =  1.0f
    };

    // The exe is rotated to match one of the axis (X axis in this case)
    // These are projections in their respectives planes
    vec3f xy_aux = { .x = exe_vec.x, .y = exe_vec.y, .z = 0 };
    vec3f xz_aux = { .x = exe_vec.x, .y = 0, .z = exe_vec.z };
    // And these are the angles they form with the vec
    float lon = get_angle3f(xz_aux, z_dir_vec3f);
    if (lon < 1) // If there is no angle (zero vector involved)
        lon = 0.0f;
    float lat = get_angle3f(xy_aux, x_dir_vec3f);
    if (lat < 1)
        lat = 0.0f;

    mat4x4f rotation_matrix_y =
    {
        .a1 = cos(lon),     .b1 = 0.0f,      .c1 = sin(lon),     .d1 =  0.0f,
        .a2 = 0.0f,         .b2 = 1.0f,      .c2 = 0.0f,         .d2 =  0.0f,
        .a3 = -sin(lon),    .b3 = 0.0f,      .c3 = cos(lon),     .d3 =  0.0f,
        .a4 = 0.0f,         .b4 = 0.0f,      .c4 = 0.0f,         .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, rotation_matrix_y);
    mat4x4f un_rotation_matrix_y = 
    {
        .a1 = cos(-lon),    .b1 = 0.0f,      .c1 = sin(-lon),    .d1 =  0.0f,
        .a2 = 0.0f,         .b2 = 1.0f,      .c2 = 0.0f,         .d2 =  0.0f,
        .a3 = -sin(-lon),   .b3 = 0.0f,      .c3 = cos(-lon),    .d3 =  0.0f,
        .a4 = 0.0f,         .b4 = 0.0f,      .c4 = 0.0f,         .d4 =  1.0f
    };
    // These matrices will be used to unmake the auxiliar rotations
    mat4x4f rotation_matrix_z =
    {
        .a1 = cos(lat),    .b1 = -sin(lat),  .c1 = 0.0f,         .d1 =  0.0f,
        .a2 = sin(lat),    .b2 = cos(lat) ,  .c2 = 0.0f,         .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = 0.0f,       .c3 = 1.0f,         .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,       .c4 = 0.0f,         .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, rotation_matrix_z);
    mat4x4f un_rotation_matrix_z =
    {
        .a1 = cos(-lat),   .b1 = -sin(-lat),  .c1 = 0.0f,        .d1 =  0.0f,
        .a2 = sin(-lat),   .b2 = cos(-lat) ,  .c2 = 0.0f,        .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = 0.0f,        .c3 = 1.0f,        .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,        .c4 = 0.0f,        .d4 =  1.0f
    };
    // With the newt rotations the exe will be overlapping the X axis, so the rotation
    // will be performed arround it. This is the transformation that won't be undone
    mat4x4f rotation_matrix_x =
    {
        .a1 = 1.0f,        .b1 = 0.0f,        .c1 = 0.0f,        .d1 =  0.0f,
        .a2 = 0.0f,        .b2 = cos(angle),  .c2 = -sin(angle), .d2 =  0.0f,
        .a3 = 0.0f,        .b3 = sin(angle),  .c3 = cos(angle),  .d3 =  0.0f,
        .a4 = 0.0f,        .b4 = 0.0f,        .c4 = 0.0f,        .d4 =  1.0f
    };
    aux = mat4x4f_prod(aux, rotation_matrix_x);

    // The auxilliar transformations are unmade in reverse order
    aux = mat4x4f_prod(aux, un_rotation_matrix_z);
    aux = mat4x4f_prod(aux, un_rotation_matrix_y);
    aux = mat4x4f_prod(aux, un_translation_matrix);

    push(active_mat, aux);
}

#endif
