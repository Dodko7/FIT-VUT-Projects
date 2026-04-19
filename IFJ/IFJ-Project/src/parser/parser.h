/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "../utils/token_types.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"

// Global variables
extern Token_ptr current_token;
extern Scanner_ptr scanner;
extern token_type fn_ret_type;
extern bool ifj_flag;
extern bool first_parse_done;
extern int statement_index;

// Function declarations
void print_token();
void next_token();
void expect_type(token_type type);
void expect_types(int count, ...);
void expect_attribute(const char* attr);


// Grammar functions
void program();
void prolog();
void function();
void function_next();
void parameter();
void parameter_next();
void body();
void statement();
void id_statement();
void value();
void not_null_value();
void then();
void else_then();
void id_continue();
void return_value();
void call();
void call_params();
void call_value();
void call_params_next();
void return_type();
void type();
void definition();

// Precedence parsing
extern char precedence_table[17][17];
int get_precedence_index(token_type type);
int getPrecedence(token_type stackTop, token_type input);
void push_operator(token_type op, token_type* operator_stack, int* operator_top);
token_type pop_operator(token_type* operator_stack, int* operator_top);
void push_operand(Token_ptr operand, Token_ptr* operand_stack, int* operand_top);
Token_ptr pop_operand(Token_ptr* operand_stack, int* operand_top);
char get_precedence_relation(token_type stack_top, token_type current);
void parse_expression();

void parse();
void parse_fn_first();

#endif // PARSER_H