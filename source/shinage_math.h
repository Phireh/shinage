#ifndef SHINAGE_MATH_H
#define SHINAGE_MATH_H
#include <math.h>
#include "shinage_debug.h"

/* Rough estimate of an epsilon value based on the ~7 digit precision of 
   fp IEEE 754. Might need revision later */
#define epsilon 0.0000001f

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

static inline vec3f hadamard_product3f(vec3f v1, vec3f v2)
{
    vec3f result = { .x = v1.x * v2.x, .y = v1.y * v2.y, .z = v1.z * v2.z };
    return result;
}

static inline vec3f sum3f(vec3f v1, vec3f v2)
{
    vec3f result = { .x = v1.x + v2.x, .y = v1.y + v2.y, .z = v1.z + v2.z };
    return result;
}

static inline vec3f diff3f(vec3f v1, vec3f v2)
{
    vec3f result = { .x = v1.x - v2.x, .y = v1.y - v2.y, .z = v1.z - v2.z };
    return result;
}

static inline float length3f(vec3f vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

static inline vec3f normalize3f(vec3f vec)
{
    float length = length3f(vec);
    if (length > 0.0f)
    {
      vec.x /= length;
      vec.y /= length;
      vec.z /= length;
    }          
    return vec;
}

/* Special clamp to keep degrees between [0,360] but wrapping the values around */
static inline float clamp_deg(float angle)
{
  return angle > 360.0f ? angle - 360.0f : angle < 0.0f ? angle + 360.0f : angle;
}

/* Rotate vector v by d degrees around the X axis (pitch) */
static inline vec3f x_axis_rot(vec3f v, float d)
{
    float angle = deg_to_rad(d);
    vec3f new_vec;
    new_vec.x = v.x;
    new_vec.y = v.y * cos(angle) - v.z * sin(angle);
    new_vec.z = v.y * sin(angle) + v.z * cos(angle);    
    return new_vec;
}

/* Rotate vector v by d degrees around the Y axis (yaw) */
static inline vec3f y_axis_rot(vec3f v, float d)
{
    float angle = deg_to_rad(d);    
    vec3f new_vec;
    new_vec.x =  v.x * cos(angle) + v.z * sin(angle);
    new_vec.y =  v.y;
    new_vec.z = -v.x * sin(angle) + v.z * cos(angle);
    return new_vec;
}

/* Rotate vector v by d degrees around the Z axis (roll) */
static inline vec3f z_axis_rot(vec3f v, float d)
{
    float angle = deg_to_rad(d);
    vec3f new_vec;
    new_vec.x = v.x * cos(angle) - v.y * sin(angle);
    new_vec.y = v.x * sin(angle) + v.y * cos(angle);
    new_vec.z = v.z;
    return new_vec;
}


#endif
