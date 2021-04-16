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

typedef struct
{
    vec3f pnt;
    vec3f vec;
} exe3f_t;

const vec3f x_dir_vec3f = { .x = 1, .y = 0, .z = 0 };
const vec3f y_dir_vec3f = { .x = 0, .y = 1, .z = 0 };
const vec3f z_dir_vec3f = { .x = 0, .y = 0, .z = 1 };
const vec3f zero_vec3f  = { .x = 0, .y = 0, .z = 0 };

const vec4f zero_vec4f  = { .x = 0, .y = 0, .z = 0, .w = 0 };

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

const mat4x4f zero_matrix_4x4 = {
  .a1 = 0, .b1 = 0, .c1 = 0, .d1 = 0,
  .a2 = 0, .b2 = 0, .c2 = 0, .d2 = 0,
  .a3 = 0, .b3 = 0, .c3 = 0, .d3 = 0,
  .a4 = 0, .b4 = 0, .c4 = 0, .d4 = 0
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

/*
*   The return value is represented in radians. If there is no solution
*   (zero vector involved), -2 is returned, a normally impossible result
*/
static inline float get_angle3f(vec3f v1, vec3f v2)
{
    float mod_v1 = length3f(v1);
    float mod_v2 = length3f(v2);
    float den = mod_v1 * mod_v2;
    if (den == 0)
      return -2;
    float dot_prod = dot_product3f(v1, v2);
    float cs = dot_prod / den;
    if (cs > 1)
      cs = 1;
    else if (cs < -1)
      cs = -1;
    float res = acos( cs );
    return res;
}

static inline vec4f sum4f(vec4f v1, vec4f v2)
{
    vec4f result = { .x = v1.x + v2.x, .y = v1.y + v2.y, .z = v1.z + v2.z , .w = v1.w + v2.w };
    return result;
}

static inline vec4f diff4f(vec4f v1, vec4f v2)
{
    vec4f result = { .x = v1.x - v2.x, .y = v1.y - v2.y, .z = v1.z - v2.z , .w = v1.w - v2.w };
    return result;
}

static inline float length4f(vec4f vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

static inline vec4f normalize4f(vec4f vec)
{
    float length = length4f(vec);
    if (length > 0.0f)
    {
      vec.x /= length;
      vec.y /= length;
      vec.z /= length;
      vec.w /= length;
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

static inline mat4x4f mat4x4f_prod(mat4x4f m1, mat4x4f m2)
{
  mat4x4f res = zero_matrix_4x4;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 4; k++)
        res.v[i * 4 + j] += m1.v[i * 4 + k] * m2.v[j + k * 4];
  return res;
}

static inline vec4f mat4x4f_vec4f_prod(mat4x4f m, vec4f v)
{
  vec4f res = zero_vec4f;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      res.v[i] += m.v[i * 4 + j] * v.v[i];
  return res;
}

static inline mat4x4f int_mat4x4f_prod(int i, mat4x4f m)
{
  mat4x4f res = m;
  for (int j = 0; j < 16; j++)
      res.v[j] = i * m.v[j];
  return res;
}

static inline mat4x4f summat4x4f(mat4x4f m1, mat4x4f m2, bool substraction)
{
  mat4x4f res = m1;
  for (int i = 0; i < 16; i++)
    if (substraction)
      res.v[i] -= i * m2.v[i];
    else
      res.v[i] += i * m2.v[i];
  return res;
}

static inline void log_debug_matx4f(mat4x4f *m, char* txt)
{
  log_debug("%s\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f\n",
                  txt,
                  m->a1, m->b1, m->c1, m->d1,
                  m->a2, m->b2, m->c2, m->d2,
                  m->a3, m->b3, m->c3, m->d3,
                  m->a4, m->b4, m->c4, m->d4);
}

static inline void log_debug_vec4f(vec4f *vs, uint count, char* txt)
{
  const int MAX_BUF = 30 * count;
  char buffer[MAX_BUF];
  uint length = 0;
  for (uint i = 0; i < count; i++)
  {
    vec4f v = vs[i];
    length += snprintf(buffer+length, MAX_BUF-length,"%2.3f\t\t%2.3f\t\t%2.3f\t\t%2.3f\n", v.x, v.y, v.z, v.w);
  }
  log_debug("\n================================================================ \
    \n%s\n%s================================================================\n", txt, buffer);
}

#endif
