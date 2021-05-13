#ifndef SHINAGE_MATH_H
#define SHINAGE_MATH_H
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
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
    struct {
        float pitch;
        float yaw;
        float roll;
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
} axis3f_t;

const vec3f x_dir_vec3f = { .x = 1, .y = 0, .z = 0 };
const vec3f y_dir_vec3f = { .x = 0, .y = 1, .z = 0 };
const vec3f z_dir_vec3f = { .x = 0, .y = 0, .z = 1 };
const vec3f zero_vec3f  = { .x = 0, .y = 0, .z = 0 };
const vec3f nan_vec3f  = { .x = NAN, .y = NAN, .z = NAN };

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
const vec3f up_vector_alt = { .x = 0, .y = 0, .z = 1 };

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
 *   (zero vector involved) -1 is returned (usually only positive angles are returned)
 */
static inline float get_angle3f(vec3f v1, vec3f v2)
{
    float mod_v1 = length3f(v1);
    float mod_v2 = length3f(v2);
    float den = mod_v1 * mod_v2;
    if (den == 0)
        return -1;
    float dot_prod = dot_product3f(v1, v2);
    float cs = dot_prod / den;
    if (cs > 1)
        cs = 1;
    else if (cs < -1)
        cs = -1;
    float res = acos( cs );
    return res;
}

int vec4_eq(vec4f v1, vec4f v2)
{
    int res = 1;
    for (int i = 0; i < 4; ++i)
        if (fabs(v1.v[i] - v2.v[i]) > epsilon)
        {
            res = 0;
        }

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
            res.v[i] += m.v[i * 4 + j] * v.v[j];
    return res;
}

static inline mat4x4f scalar_mat4x4f_prod(float sc, mat4x4f m)
{
    mat4x4f res = m;
    for (int j = 0; j < 16; j++)
        res.v[j] = sc * m.v[j];
    return res;
}

static inline mat4x4f summat4x4f(mat4x4f m1, mat4x4f m2, bool subtraction)
{
    mat4x4f res = m1;
    int sign = subtraction == 0 ? 1 : -1;
    for (int i = 0; i < 16; i++)
        res.v[i] += sign * m2.v[i];

    return res;
}

static inline void log_debug_matx2f(mat2x2f *m, char* txt)
{
    log_debug("%s\n %.3f %.3f\n %.3f %.3f",
              txt,
              m->a1, m->b1,
              m->a2, m->b2);
}

static inline void log_debug_matx3f(mat3x3f *m, char* txt)
{
    log_debug("%s\n %.3f %.3f %.3f\n %.3f %.3f %.3f\n %.3f %.3f %.3f",
              txt,
              m->a1, m->b1, m->c1,
              m->a2, m->b2, m->c2,
              m->a3, m->b3, m->c3);
}

static inline void log_debug_matx4f(mat4x4f *m, char* txt)
{
    log_debug("%s\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f\n %.3f %.3f %.3f %.3f",
              txt,
              m->a1, m->b1, m->c1, m->d1,
              m->a2, m->b2, m->c2, m->d2,
              m->a3, m->b3, m->c3, m->d3,
              m->a4, m->b4, m->c4, m->d4);
}

static inline void log_debug_vec4f(vec4f *vs, uint count, char* txt)
{
    const int MAX_BUF = 32 * count;
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

static inline float determinant_mat2x2f(mat2x2f m)
{
    return m.a1 * m.b2 - m.b1 * m.a2;
}

static inline float determinant_mat3x3f(mat3x3f m, uint pivot_row)
{
    float res = 0;
    uint row_ids[] = { 1, 2 };
    for (uint i = 0; i < pivot_row; i++)
        row_ids[i]--;
    for (uint i = 0; i < 3; i++)
        {
            float val = m.rows[pivot_row].v[i];
            if (val == 0)
                continue;
            uint col_ids[] = { 1, 2 };
            for (uint j = 0; j < i; j++)
                col_ids[j]--;
            mat2x2f sub_mat =
                {
                    .a1 = m.rows[row_ids[0]].v[col_ids[0]], .b1 = m.rows[row_ids[0]].v[col_ids[1]],
                    .a2 = m.rows[row_ids[1]].v[col_ids[0]], .b2 = m.rows[row_ids[1]].v[col_ids[1]]
                };
            float det = determinant_mat2x2f(sub_mat);
            float sig = ((pivot_row + i) % 2 == 0) ? 1 : -1;
            res += det * val * sig;
        }
    return res;
}

static inline float determinant_mat4x4f(mat4x4f m, uint pivot_row)
{
    float res = 0;
    uint row_ids[] = { 1, 2, 3 };
    for (uint i = 0; i < pivot_row; i++)
        row_ids[i]--;
    for (uint i = 0; i < 4; i++)
        {
            float val = m.rows[pivot_row].v[i];
            if (val == 0)
                continue;
            uint col_ids[] = { 1, 2, 3 };
            for (uint j = 0; j < i; j++)
                col_ids[j]--;
            mat3x3f sub_mat =
                {
                    .a1 = m.rows[row_ids[0]].v[col_ids[0]], .b1 = m.rows[row_ids[0]].v[col_ids[1]], .c1 = m.rows[row_ids[0]].v[col_ids[2]],
                    .a2 = m.rows[row_ids[1]].v[col_ids[0]], .b2 = m.rows[row_ids[1]].v[col_ids[1]], .c2 = m.rows[row_ids[1]].v[col_ids[2]],
                    .a3 = m.rows[row_ids[2]].v[col_ids[0]], .b3 = m.rows[row_ids[2]].v[col_ids[1]], .c3 = m.rows[row_ids[2]].v[col_ids[2]],
                };
            float det = determinant_mat3x3f(sub_mat, 0);
            float sig = ((pivot_row + i) % 2 == 0) ? 1 : -1;
            res += det * val * sig;
        }
    return res;
}

static inline mat4x4f adjoint_mat4x4f(mat4x4f m, bool det_check)
{
    if (det_check)
        {
            float det = determinant_mat4x4f(m, 0);
            log_debug("Det value = %f", det);
            if (det == 0)
                // TODO: Find a good and cheap way to aknowledge a failed matrix operation (NaN matrix?)
                return zero_matrix_4x4;
        }
    mat4x4f adjoint =
        {
            .a1 = 1,  .b1 = -1, .c1 = 1,   .d1 = -1,
            .a2 = -1, .b2 = 1,  .c2 = -1,  .d2 = 1,
            .a3 = 1,  .b3 = -1, .c3 = 1,   .d3 = -1,
            .a4 = -1, .b4 = 1,  .c4 = -1,  .d4 = 1
        };
    for (uint i = 0; i < 4; i++)
        {
            uint row_ids[] = { 1, 2, 3 };
            for (uint k = 0; k < i; k++)
                row_ids[k]--;
            for (uint j = 0; j < 4; j++)
                {
                    uint col_ids[] = { 1, 2, 3 };
                    for (uint l = 0; l < j; l++)
                        col_ids[l]--;
                    mat3x3f aux =
                        {
                            .a1 = m.rows[row_ids[0]].v[col_ids[0]],  .b1 = m.rows[row_ids[0]].v[col_ids[1]], .c1 = m.rows[row_ids[0]].v[col_ids[2]],
                            .a2 = m.rows[row_ids[1]].v[col_ids[0]],  .b2 = m.rows[row_ids[1]].v[col_ids[1]], .c2 = m.rows[row_ids[1]].v[col_ids[2]],
                            .a3 = m.rows[row_ids[2]].v[col_ids[0]],  .b3 = m.rows[row_ids[2]].v[col_ids[1]], .c3 = m.rows[row_ids[2]].v[col_ids[2]]
                        };
                    adjoint.rows[i].v[j] *= determinant_mat3x3f(aux, 0);
                }
        }
    return adjoint;
}

static inline mat4x4f transpose_mat4x4f(mat4x4f m)
{
    mat4x4f res =
        {
            .a1 = m.a1, .b1 = m.a2, .c1 = m.a3, .d1 = m.a4,
            .a2 = m.b1, .b2 = m.b2, .c2 = m.b3, .d2 = m.b4,
            .a3 = m.c1, .b3 = m.c2, .c3 = m.c3, .d3 = m.c4,
            .a4 = m.d1, .b4 = m.d2, .c4 = m.d3, .d4 = m.d4
        };
    return res;
}

static inline mat4x4f inverse_mat4x4f(mat4x4f m, bool det_check, bool gauss)
{
    float det;
    if (det_check || !gauss)
        {
            det = determinant_mat4x4f(m, 0);
            log_debug("Det value = %f", det);
            if (det == 0)
                // TODO: Find a good and cheap way to aknowledge a failed matrix operation (NaN matrix?)
                return zero_matrix_4x4;
        }

    if (gauss)
        {
            int i, j;
            // rows x columns
            float *aux[4] =
                {
                    (float*)alloca(sizeof(float) * 8),
                    (float*)alloca(sizeof(float) * 8),
                    (float*)alloca(sizeof(float) * 8),
                    (float*)alloca(sizeof(float) * 8)
                };
            for (i = 0; i < 4; i++)
                memset(aux[i], 0.0f, sizeof(float) * 8);
            // Setting the aumented matrix
            aux[0][4] = 1;
            aux[1][5] = 1;
            aux[2][6] = 1;
            aux[3][7] = 1;
            for (i = 0; i < 4; i++)
                for (j = 0; j < 4; j++)
                    aux[i][j] = m.rows[i].v[j];
            /* Ordering the rows of matrix,
               The interchanges will start from the last row */
            for (i = 3; i > 1; i--)
                {
                    if (aux[i - 1][0] < aux[i][0]) {
                        float* temp = aux[i];
                        aux[i] = aux[i - 1];
                        aux[i - 1] = temp;
                    }
                }

            /* Replace a row by sum of itself and a
               constant multiple of another row of the matrix */
            for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                        {
                            if (j != i)
                                {
                                    float temp = aux[j][i] / aux[i][i];
                                    for (int k = 0; k < 8; k++) {
                                        aux[j][k] -= aux[i][k] * temp;
                                    }
                                }
                        }
                }
            /* Multiply each row by a nonzero integer.
               Divide row element by the diagonal element */
            for (int i = 0; i < 4; i++) {
                float temp = aux[i][i];
                for (int j = 0; j < 8; j++) {

                    aux[i][j] = aux[i][j] / temp;
                }
            }

            mat4x4f res =
                {
                    .a1 = aux[0][4],   .b1 = aux[0][5],   .c1 = aux[0][6],   .d1 =  aux[0][7],
                    .a2 = aux[1][4],   .b2 = aux[1][5],   .c2 = aux[1][6],   .d2 =  aux[1][7],
                    .a3 = aux[2][4],   .b3 = aux[2][5],   .c3 = aux[2][6],   .d3 =  aux[2][7],
                    .a4 = aux[3][4],   .b4 = aux[3][5],   .c4 = aux[3][6],   .d4 =  aux[3][7]
                };
            return res;
        }
    else
        {
            mat4x4f res = scalar_mat4x4f_prod(1.0f / det, transpose_mat4x4f(adjoint_mat4x4f(m, false)));
            return res;
        }
}

static inline mat4x4f get_rotation_mat4x4f(mat4x4f m)
{
    mat4x4f aux =
    {
        .a1 = m.a1, .b1 = m.a2, .c1 = m.a3, .d1 = 0,
        .a2 = m.b1, .b2 = m.b2, .c2 = m.b3, .d2 = 0,
        .a3 = m.c1, .b3 = m.c2, .c3 = m.c3, .d3 = 0,
        .a4 = 0,    .b4 = 0,    .c4 = 0,    .d4 = 1
    };
    return inverse_mat4x4f(aux, false, false);
}

/* Orthogonal projection matrix with an infinite clip, atm used for text */
mat4x4f orthogonal_proj_matrix(float left, float right, float bottom, float top)
{
    mat4x4f ortho = {
        .a1 = 2/(right - left),
        .b2 = 2/(top - bottom),
        .c3 = 1,
        .d1 = -(right + left)/(right - left),
        .d2 = -(top + bottom)/(top-bottom),
        .d4 = 1
    };
    return ortho;
}

#endif
