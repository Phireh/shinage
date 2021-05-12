#ifndef SHINAGE_CAMERA_H
#define SHINAGE_CAMERA_H

#include "shinage_matrices.h"
#include "shinage_stack_structures.h"

typedef struct
{
    matrix_stack_t *view;
    matrix_stack_t *projection;
} camera_t;

#endif
