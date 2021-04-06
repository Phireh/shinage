#ifndef SHINAGE_MATH_H
#define SHINAGE_MATH_H
#include <math.h>
#include "shinage_debug.h"


typedef union {
  // Access as components
  struct {
    float x;
    float y;
  };
  // Access as raw values
  float v[2];
} vec2f;

typedef union {
  // Access as vec components
  vec2f rows[2];
  // Access as individual components
  struct {
    float a1;
    float b1;
    float a2;
    float b2;
  };
  float v[2*2];
} mat2x2f;

typedef union {
  // Access as components
  struct {
    float x;
    float y;
    float z;
  };
  // Access as raw values
  float v[3];
} vec3f;

typedef union {
  // Access as vector components
  vec3f rows[3];
  // Access as individual components
  struct {
    float a1;
    float b1;
    float c1;
    float a2;
    float b2;
    float c2;
    float a3;
    float b3;
    float c3;
  };
  // Access as raw values
  float v[3*3];
} mat3x3f;


typedef union {
    // Access as components
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    // Access as raw values
    float v[4];
} vec4f;

typedef union {
    // Access as vector components
    vec4f rows[4];
    // Access as individual components
    struct {
        // First vec
        float a1;
        float b1;
        float c1;
        float d1;
        // Second vec
        float a2;
        float b2;
        float c2;
        float d2;
        // Third vec
        float a3;
        float b3;
        float c3;
        float d3;
        // Fourth vec
        float a4;
        float b4;
        float c4;
        float d4;
    };
    // Access as raw values
  float v[4*4];    
} mat4x4f;

const mat2x2f identity_matrix_2x2 = {
  .a1 = 1, .b1 = 0,
  .a2 = 0, .b2 = 1,
};

const mat3x3f identity_matrix_3x3 = {
  .a1 = 1, .b1 = 0, .c1 = 0,
  .a2 = 0, .b2 = 1, .c2 = 0,
  .a3 = 0, .b3 = 0, .c3 = 1
};

const mat4x4f identity_matrix_4x4 = {
  .a1 = 1, .b1 = 0, .c1 = 0, .d1 = 0,
  .a2 = 0, .b2 = 1, .c2 = 0, .d2 = 0,
  .a3 = 0, .b3 = 0, .c3 = 1, .d3 = 0,
  .a4 = 0, .b4 = 0, .c4 = 0, .d4 = 1
};

const vec3f up_vector = { .x = 0, .y = 1, .z = 0 };

static inline float deg_to_rad(float degrees)
{
    return (degrees * M_PI) / 180.0f;
}

static inline float rad_to_deg(float radians)
{
    return (radians * 180.0f) / M_PI;
}

/* TODO: Test this */
static inline vec3f cross_product3f(vec3f v1, vec3f v2)
{
    vec3f cross_product = {
        .x = v1.y * v2.z - v1.z * v2.y,
        .y = v1.z * v2.x - v1.x * v2.z,
        .z = v1.x * v2.y - v1.y * v2.x
    };

    return cross_product;        
}

static inline float dot_product3f(vec3f v1, vec3f v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static inline vec3f normalize3f(vec3f vec)
{
    float length = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    vec.x /= length;
    vec.y /= length;
    vec.z /= length;
    return vec;
}

#endif
