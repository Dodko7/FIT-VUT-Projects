#include "token.h"

#ifndef STACK_H
#define STACK_H

#define MAX 200

/*
 * Structure for stack
 */
typedef struct {
    Token_ptr items[MAX];
    int top;
} Stack;

void init(Stack *stack);
int isEmpty(Stack *stack);
void push(Stack *stack, Token_ptr value);
Token_ptr pop(Stack *stack);
Token_ptr peek(Stack *stack);

#endif