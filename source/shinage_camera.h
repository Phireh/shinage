#ifndef SHINAGE_CAMERA_H
#define SHINAGE_CAMERA_H

#include "shinage_stack_structures.h"
#include "shinage_debug.h"

typedef struct
{
    matrix_stack_t *view;
    matrix_stack_t *projection;
} camera_t;

void set_look_at_camera(camera_t cam, vec3f e, vec3f poi, vec3f up)
{
	pop(cam.view);
	mat4x4f mat = get_look_at_mat4x4f(e, poi, up);
	push(cam.view, mat);
}

void set_perspective_camera(camera_t cam, float fov_y, float ar, float n, float f)
{
	pop(cam.projection);
	mat4x4f mat = get_perspective_camera_mat4x4f(fov_y, ar, n, f);
	push(cam.projection, mat);
}

/* Convenience function that takes into account the View matrix Z coord
   orientation, see notes on add_translation */
void translate_camera(camera_t cam, float x, float y, float z)
{
	if (!cam.view)
		log_err("Error: trying to translate a non initiallized camera");
        return;

    mat4x4f mat = pop(cam.view);
    vec3f aux = { .x = x, .y = y, .z = z };
    mat = get_translated_matrix_mat4x4f(mat, aux);
    push(cam.view, mat);
}

void add_pitch_camera(camera_t cam, float angle)
{
	if (!cam.view)
		log_err("Error: trying to add pitch to a non initiallized camera");
        return;

    mat4x4f mat = pop(cam.view);
    mat = get_added_pitch_mat4x4f(mat, angle);
    push(cam.view, mat);
}

void add_yaw_camera(camera_t cam, float angle, bool world_axis)
{
	if (!cam.view)
		log_err("Error: trying to add yaw to a non initiallized camera");
        return;

    mat4x4f mat = pop(cam.view);
    if (world_axis)
		mat = get_added_yaw_world_axis_mat4x4f(mat, angle);
	else
		mat = get_added_yaw_mat4x4f(mat, angle);
    push(cam.view, mat);
}

void add_roll_camera(camera_t cam, float angle)
{
	if (!cam.view)
		log_err("Error: trying to add roll to a non initiallized camera");
        return;

    mat4x4f mat = pop(cam.view);
    mat = get_added_roll_mat4x4f(mat, angle);
    push(cam.view, mat);
}

static inline vec3f get_position_camera(camera_t cam)
{
	if (!cam.view)
		log_err("Error: trying to get the position from a non initiallized camera");
        return nan_vec3f;

    mat4x4f mat = peek(cam.view);
    return get_position_mat4x4f(mat);
}

#endif
