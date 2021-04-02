#ifndef SHINAGE_CAMERA_H
#define SHINAGE_CAMERA_H
#include "shinage_math.h"

/* TODO: Check if operations on this struct are more performant when passed as reference
   instead of doing it by value */
typedef struct {
    vec3f position;
    vec3f target;
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
    // ...
}

#endif
