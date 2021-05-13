#ifndef SHINAGE_STACK_STRUCTURES_H
#define SHINAGE_STACK_STRUCTURES_H

#include "shinage_math.h"
#include "shinage_debug.h"
#include "shinage_stack_structures.h"

#include <stdlib.h>
 
typedef struct
{
    int top; // index
    unsigned int capacity;
    mat4x4f *array;
} matrix_stack_t;
 
matrix_stack_t* build_stack(unsigned int capacity)
{
    matrix_stack_t *stack = (matrix_stack_t*)malloc(sizeof(matrix_stack_t));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (mat4x4f*)malloc(capacity * sizeof(mat4x4f));
    return stack;
}
 
int is_full(matrix_stack_t *stack)
{
    return stack->top == (int)(stack->capacity) - 1;
}
 
int is_empty(matrix_stack_t *stack)
{
    return stack->top == -1;
}
 
bool push(matrix_stack_t *stack, mat4x4f item)
{
    if (is_full(stack))
        return false;
    stack->array[++stack->top] = item;
    return true;
}
 
mat4x4f pop(matrix_stack_t *stack)
{
    if (is_empty(stack))
        return identity_matrix_4x4;
    return stack->array[stack->top--];
}
 
mat4x4f peek(matrix_stack_t *stack)
{
    if (is_empty(stack))
        return identity_matrix_4x4;
    return stack->array[stack->top];
}



#endif