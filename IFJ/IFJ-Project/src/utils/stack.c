#include <stdio.h>
#include "stack.h"

/*
 * Initialisation of stack
 */
void init(Stack *stack) {
    stack->top = -1;
}

/* 
 * Function for empty stack
 */
int isEmpty(Stack *stack) {
    return stack->top == -1;
}
/* 
 * Function to push operator to stack
 */
void push(Stack *stack, Token_ptr value) {
    if (stack->top == MAX - 1) {
        fprintf(stderr, "Stack overflow\n");
        return;
    }
    stack->items[++stack->top] = value;
}

/* 
 * Function to pop operator from stack
 */
Token_ptr pop(Stack *stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Stack underflow\n");
        return NULL;
    }
    return stack->items[stack->top--];
}

/* 
 * Function to peek the top of the stack
 */
Token_ptr peek(Stack *stack) {
    if (isEmpty(stack)) {
        return NULL;
    }
    return stack->items[stack->top];
}