#include <stdio.h>
#include <utest.h>

#include "shinage_common.h"
#include "shinage_debug.h"
#include "shinage_math.h"
#include "shinage_stack_structures.h"
#include "shinage_camera.h"


#define GREEN_BOLD "\033[1;32m"
#define RED_BOLD   "\033[1;31m"
#define BLUE_BOLD  "\033[1;36m"
#define PINK_BOLD  "\033[1;35m"
#define RESET_TEXT "\033[0m"

#define log_ok(str, ...) fprintf(stderr, "[" GREEN_BOLD "OK" RESET_TEXT "] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_fail(str, ...) fprintf(stderr, "[" RED_BOLD "FAILED" RESET_TEXT "] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_detail(str, ...) fprintf(stderr, "[" BLUE_BOLD "DETAIL" RESET_TEXT "] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_summary(str, ...) fprintf(stderr, "[" PINK_BOLD "SUMMARY" RESET_TEXT "] (%s:%d): " str "\n", __FILE__, __LINE__, ##__VA_ARGS__)


int mat4_eq_debug(mat4x4f m1, mat4x4f m2)
{
    int res = 1;
    for (int i = 0; i < 16; ++i)
    {
        if (isnan(m1.v[i]) || (fabs(m1.v[i] - m2.v[i]) > epsilon))
        {
            log_detail("At position %d -> %f should be equal to %f", i, m1.v[i], m2.v[i]);
            res =  0;
        }
    }

    return res;
}

int vec4_eq_debug(vec4f v1, vec4f v2)
{
    int res = 1;
    for (int i = 0; i < 4; ++i)
        if (fabs(v1.v[i] - v2.v[i]) > epsilon)
        {
            log_detail("At position %d -> %f should be equal to %f", i, v1.v[i], v2.v[i]);
            res = 0;
        }

    return res;
}

int vec3_eq(vec3f v1, vec3f v2)
{
    int res = 1;
    for (int i = 0; i < 3; ++i)
        if (fabs(v1.v[i] - v2.v[i]) > epsilon)
        {
            log_detail("At position %d -> %f should be equal to %f", i, v1.v[i], v2.v[i]);
            res = 0;
        }

    return res;
}

UTEST(matrix_math, arithmetic)
{
    /* Test shinage matrix math */
    mat4x4f m1_t1, m2_t1;
    for (int i = 0; i < 16; ++i)
    {
        m1_t1.v[i] =    i+1;
        m2_t1.v[i] = -(i+1);
    }
    /* m1 : [1, 2, 3 ...]; m2 : [ -1, -2, -3, ... ] */
    mat4x4f m3_t1 = mat4x4f_prod(m1_t1, m2_t1);

    /* Expected result of multiplication */
    mat4x4f m4_t1 = {
        .a1 = -90,  .b1 = -100, .c1 = -110, .d1 = -120,
        .a2 = -202, .b2 = -228, .c2 = -254, .d2 = -280,
        .a3 = -314, .b3 = -356, .c3 = -398, .d3 = -440,
        .a4 = -426, .b4 = -484, .c4 = -542, .d4 = -600
    };
    EXPECT_TRUE(mat4_eq_debug(m3_t1, m4_t1));

    /* Expected result of addition is the zero_matrix */
    EXPECT_TRUE(mat4_eq_debug(zero_matrix_4x4, summat4x4f(m1_t1, m2_t1, 0)));


    /* Expected result of subtraction is [ 2, 4, 6, 8 ... ] */
    mat4x4f m1_t3;
    for (int i = 0; i < 16; ++i)
        m1_t3.v[i] = (i+1)*2;

    EXPECT_TRUE(mat4_eq_debug(m1_t3, summat4x4f(m1_t1, m2_t1, 1)));

    /* Expected result of scalar multiplication is the same as previous test */
    mat4x4f m1_t4 = m1_t3;

    EXPECT_TRUE(mat4_eq_debug(m1_t4, scalar_mat4x4f_prod(2, m1_t1)));

    vec4f v1_t5 = { .x = 1, .y = 2, .z = 3, .w = 4 };

    /* Expected value of matrix x vector multiplication */
    vec4f v2_t5 = { .x = 30, .y = 70, .z = 110, .w = 150 };

    EXPECT_TRUE(vec4_eq_debug(mat4x4f_vec4f_prod(m1_t1, v1_t5), v2_t5));
}

UTEST(vector_math, rotations)
{
    vec3f v1_t6 = { .x = 1, .y = 1, .z = 0 };

    /* Expected value of rotating vector 90?? around Y axis */
    vec3f v2_t6 = { .x = 0, .y = 1, .z = -1};

    EXPECT_TRUE(vec3_eq(y_axis_rot(v1_t6, 90.0f), v2_t6));

    vec3f v1_t7 = { .x = 1, .y = 1, .z = 0 };

    /* Expected value of rotating vector 90?? around X axis */
    vec3f v2_t7 = { .x = 1, .y = 0, .z = 1};

    EXPECT_TRUE(vec3_eq(x_axis_rot(v1_t7, 90.0f), v2_t7));

    vec3f v1_t8 = { .x = 1, .y = 1, .z = 0 };

    /* Expected value of rotating vector 90?? around Z axis */
    vec3f v2_t8 = { .x = -1, .y = 1, .z = 0};

    EXPECT_TRUE(vec3_eq(z_axis_rot(v1_t8, 90.0f), v2_t8));
}

UTEST(vector_math, angle)
{
    vec3f v1_t9 = { .x = 1, .y = 0, .z = 1 };
    vec3f v2_t9 = { .x = 0, .y = 1, .z = 0 };
    /* Expected resulting angle is 90?? (M_PI/2 rads) */
    EXPECT_TRUE(get_angle3f(v1_t9, v2_t9) - (M_PI/2) < epsilon);
}

UTEST(matrix_math, inverse)
{
    /* Matrix with diagonal [ 1, 2 , 3, 4] */
    mat4x4f m1_t10 = { .a1 = 1, .b2 = 2, .c3 = 3, .d4 = 4 };
    /* Determinant should be 24 */
    EXPECT_TRUE(fabs(determinant_mat4x4f(m1_t10, 0) - 24.0f) < epsilon);

    /* Expected result of inverse matrix */
    mat4x4f m1_t11 = { .a1 = 1, .b2 = 0.5f, .c3 = (1.0f/3.0f), .d4 = 0.25f };


    mat4x4f m2_t11 = {
        .a1 = -1, .b1 = 0, .c1 = 0,  .d1 = 0,
        .a2 = 0,  .b2 = 1, .c2 = 0,  .d2 = 0,
        .a3 = 0,  .b3 = 0, .c3 = -1, .d3 = -1,
        .a4 = 0,  .b4 = 0, .c4 = 0,  .d4 = 1
    };

    /* Expected result */
    mat4x4f m3_t11 = {
        .a1 = -1, .b1 = 0, .c1 = 0,  .d1 = 0,
        .a2 = 0,  .b2 = 1, .c2 = 0,  .d2 = 0,
        .a3 = 0,  .b3 = 0, .c3 = -1, .d3 = -1,
        .a4 = 0,  .b4 = 0, .c4 = 0,  .d4 = 1
    };

    EXPECT_TRUE(mat4_eq_debug(inverse_mat4x4f(m1_t10), m1_t11));
    EXPECT_TRUE(mat4_eq_debug(inverse_mat4x4f(m2_t11), m3_t11));
}

UTEST(matrix_math, camera)
{
     /* Expected view matrix for a camera in (0,0,1) looking at (0,0,0) */
    mat4x4f m1_t12 = {
        .a1 = 1,
        .b2 = 1,
        .c3 = 1, .d3 = -1,
        .d4 = 1
    };


    game_state_t g = {};
    update_global_vars(&g);
    build_matrices();
    set_mat(VIEW, &g);

    vec3f camera_pos = { .x = 0, .y = 0, .z = 1 };
    vec3f target = { .x = 0, .y = 0, .z = 0 };
    vec3f up = { .x = 0, .y = 1, .z = 0 };
    look_at(camera_pos, target, up);

    mat4x4f m2_t12 = peek(mats->view);

    EXPECT_TRUE(mat4_eq_debug(m2_t12, m1_t12));

    /* Expected result of asking for the camera position */
    vec3f v1_t13 = camera_pos;
    EXPECT_TRUE(vec3_eq(get_position(), v1_t13));
}

UTEST_MAIN();
