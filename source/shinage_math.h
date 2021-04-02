#ifndef SHINAGE_MATH_H
#define SHINAGE_MATH_H

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
  vec2f rows[3];
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

const mat2x2f identity_matrix_2x2 = {
  .a1 = 1, .b1 = 0,
  .a2 = 0, .b2 = 1,
};

const mat3x3f identity_matrix_3x3 = {
  .a1 = 1, .b1 = 0, .c1 = 0,
  .a2 = 0, .b2 = 1, .c2 = 0,
  .a3 = 0, .b3 = 0, .c3 = 1
};

#endif
