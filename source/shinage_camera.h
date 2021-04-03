#ifndef SHINAGE_CAMERA_H
#define SHINAGE_CAMERA_H
#include "shinage_math.h"

/* TODO: Check if operations on this struct are more performant when passed as reference
   instead of doing it by value */
typedef struct {
    vec3f pos;
    vec3f target;
    vec3f up;
    float fov;
    float near;
    float far;
    unsigned int viewport_w;
    unsigned int viewport_h;
} camera_t;

static inline mat4x4f proj_matrix(camera_t camera)
{
    /* TODO: Get better names for these */
    float far = camera.far;
    float near = camera.near;
    float fn = far + near;
    float nf = far - near;
    float r = (float)camera.viewport_w / (float)camera.viewport_h;
    float t = 1.0f / (tan(deg_to_rad(camera.fov)) / 2.0f);


    mat4x4f matrix = {
        .a1 = t/r,  .b1 = 0.0f, .c1 = 0.0f,   .d1 =  0.0f,
        .a2 = 0.0f, .b2 = t,    .c2 = 0.0f,   .d2 =  0.0f,
        .a3 = 0.0f, .b3 = 0.0f, .c3 = -fn/nf, .d3 = -1.0f,
        .a4 = 0.0f, .b4 = 0.0f, .c4 = -2.0 * far * near / nf, 0.0f
    };
    return matrix;
}

static inline mat4x4f view_matrix(camera_t camera)
{
    vec3f mz = { .x = camera.pos.x - camera.target.x,
                 .y = camera.pos.y - camera.target.y,
                 .z = camera.pos.z - camera.target.z
    };
    mz = normalize3f(mz);
    
    vec3f my = camera.up;
    vec3f mx = cross_product3f(my, mz);
    mx = normalize3f(mx);
    my = cross_product3f(mz, my);
    vec3f _mz = { .x = -mz.x, .y = -mz.y, .z = -mz.z };
    vec4f last_row = {
        .x = dot_product3f(mx, camera.pos),
        .y = dot_product3f(my, camera.pos),
        .z = dot_product3f(_mz, camera.pos),
        .w = 1.0f
    };

    mat4x4f matrix = {
        .a1 = mx.x, .b1 = my.x, .c1 = mz.x, .d1 = 0.0f,
        .a2 = mx.y, .b2 = my.y, .c2 = mz.y, .d2 = 0.0f,
        .a3 = mx.z, .b3 = my.z, .c3 = mz.z, .d3 = 0.0f,
        .rows[3] = last_row
    };
    return matrix;
}

#endif
