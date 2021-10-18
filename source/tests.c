#include <stdio.h>

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

int main()
{


    int fail_count = 0;
    int ok_count = 0;

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

    if (mat4_eq_debug(m3_t1, m4_t1))
    {
        ++ok_count;
        log_ok("Matrix multiplication");
    }
    else
    {
        ++fail_count;
        log_fail("Matrix multiplication");
    }

    /* Expected result of addition is the zero_matrix */
    if (mat4_eq_debug(zero_matrix_4x4, summat4x4f(m1_t1, m2_t1, 0)))
    {
        ++ok_count;
        log_ok("Matrix addition");
    }
    else
    {
        ++fail_count;
        log_fail("Matrix addition");
    }

    /* Expected result of subtraction is [ 2, 4, 6, 8 ... ] */
    mat4x4f m1_t3;
    for (int i = 0; i < 16; ++i)
        m1_t3.v[i] = (i+1)*2;

    if (mat4_eq_debug(m1_t3, summat4x4f(m1_t1, m2_t1, 1)))
    {
        ++ok_count;
        log_ok("Matrix subtraction");
    }
    else
    {
        ++fail_count;
        log_fail("Matrix subtraction");
    }

    /* Expected result of scalar multiplication is the same as previous test */
    mat4x4f m1_t4 = m1_t3;
    if (mat4_eq_debug(m1_t4, scalar_mat4x4f_prod(2, m1_t1)))
    {
        ++ok_count;
        log_ok("Matrix x scalar multiplication");
    }
    else
    {
        ++fail_count;
        log_fail("Matrix x scalar multiplication");
    }

    vec4f v1_t5 = { .x = 1, .y = 2, .z = 3, .w = 4 };

    /* Expected value of matrix x vector multiplication */
    vec4f v2_t5 = { .x = 30, .y = 70, .z = 110, .w = 150 };

    if (vec4_eq_debug(mat4x4f_vec4f_prod(m1_t1, v1_t5), v2_t5))
    {
        ++ok_count;
        log_ok("Matrix x vector multiplication");
    }
    else
    {
        ++fail_count;
        log_fail("Matrix x vector multiplication");
    }


    vec3f v1_t6 = { .x = 1, .y = 1, .z = 0 };

    /* Expected value of rotating vector 90º around Y axis */
    vec3f v2_t6 = { .x = 0, .y = 1, .z = -1};

    if (vec3_eq(y_axis_rot(v1_t6, 90.0f), v2_t6))
    {
        ++ok_count;
        log_ok("Vector Y rotation");
    }
    else
    {
        ++fail_count;
        log_fail("Vector Y rotation");
    }

    vec3f v1_t7 = { .x = 1, .y = 1, .z = 0 };

    /* Expected value of rotating vector 90º around X axis */
    vec3f v2_t7 = { .x = 1, .y = 0, .z = 1};

    if (vec3_eq(x_axis_rot(v1_t7, 90.0f), v2_t7))
    {
        ++ok_count;
        log_ok("Vector X rotation");
    }
    else
    {
        ++fail_count;
        log_fail("Vector X rotation");
    }

    vec3f v1_t8 = { .x = 1, .y = 1, .z = 0 };

    /* Expected value of rotating vector 90º around Z axis */
    vec3f v2_t8 = { .x = -1, .y = 1, .z = 0};

    if (vec3_eq(z_axis_rot(v1_t8, 90.0f), v2_t8))
    {
        ++ok_count;
        log_ok("Vector Z rotation");
    }
    else
    {
        ++fail_count;
        log_fail("Vector Z rotation");
    }

    vec3f v1_t9 = { .x = 1, .y = 0, .z = 1 };
    vec3f v2_t9 = { .x = 0, .y = 1, .z = 0 };

    /* Expected resulting angle is 90º (M_PI/2 rads) */
    if (get_angle3f(v1_t9, v2_t9) - (M_PI/2) < epsilon)
    {
        ++ok_count;
        log_ok("Angle between vectors");
    }
    else
    {
        ++fail_count;
        log_fail("Angle between vectors");
    }

    /* Matrix with diagonal [ 1, 2 , 3, 4] */
    mat4x4f m1_t10 = { .a1 = 1, .b2 = 2, .c3 = 3, .d4 = 4 };
    /* Determinant should be 24 */
    if (fabs(determinant_mat4x4f(m1_t10, 0) - 24.0f) < epsilon)
    {
        ++ok_count;
        log_ok("Matrix determinant");
    }
    else
    {
        ++fail_count;
        log_fail("Matrix determinant");
    }

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

    if (mat4_eq_debug(inverse_mat4x4f(m1_t10), m1_t11))
    {
        ++ok_count;
        log_ok("Matrix inverse non-Gauss 1/2");
    }
    else
    {
        ++fail_count;
        log_fail("Matrix inverse non-Gauss 1/2");
    }

    if (mat4_eq_debug(inverse_mat4x4f(m2_t11), m3_t11))
    {
        ++ok_count;
        log_ok("Matrix inverse non-Gauss 2/2");
    }
    else
    {
        ++fail_count;
        log_fail("Matrix inverse non-Gauss 2/2");
    }

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

    if (mat4_eq_debug(m2_t12, m1_t12))
    {
        ++ok_count;
        log_ok("View matrix");
    }
    else
    {
        ++fail_count;
        log_fail("View matrix");
    }

    /* Expected result of asking for the camera position */
    vec3f v1_t13 = camera_pos;

    if (vec3_eq(get_position(), v1_t13))
    {
        ++ok_count;
        log_ok("get_position");
    }
    else
    {
        ++fail_count;
        log_fail("get_position");
    }

    log_summary("%d successful tests, %d failed tests", ok_count, fail_count);

    return fail_count;
}
