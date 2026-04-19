/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

// TO-DO:
// 1) built-in funkcie
// 2) semantika
// 3) Iplement second parser

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "parser.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"
#include "../utils/errors.h"
#include "../utils/codegen.h"
#include "../utils/stack.h"
#include "../utils/symtable.h"
#include "../utils/symDLList.h"
#include "../utils/dymString.h"
#include "../utils/str.h"


#define OK if (error != err_none) return
/* 
 * Global variables for the parser
 */
Token_ptr current_token = NULL;
Scanner_ptr scanner = NULL;
token_type fn_ret_type;
bool ifj_flag = false;
int statement_index = 1;
err_codes error = err_none; 

#define printi(source, ...) if (strcmp(source, format[_defvar]) == 0) \
                                printi_defvar(&string, &string_defvar, source, ##__VA_ARGS__); \
                            else \
                                str_append(&string_tmp, source, ##__VA_ARGS__)
/*
 * Global variables for the code generation
 */
Stack stack_codegen;
int counter_codegen_if = 0;
int counter_codegen_while = 0;
int counter_codegen_for = 1;
int counter_codegen_expression = 0;
int counter_global = 0;
int cycle = -1;
char string_buffer[MAX_STRING_LEN];
char string_buffer_value[MAX_STRING_LEN];
str_t string;
str_t string_tmp;
str_t string_defvar;

symtable_t* current_symtable;
DLList sym_list;
data_t *left_data;
data_t *right_data;
data_t *result_data;
data_t *param_data;
data_t *function_data;
DymString *return_logic;
char stringBuffer[100] = "\0";

/*
 * Precedence table
 */
char precedence_table[17][17] = {
//     +    -    *    /    ==   !=   <    >    <=   >=  and   or   !    (    )    id   $
    { '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<', '>' }, // +
    { '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<', '>' }, // -
    { '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<', '>' }, // *
    { '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<', '>' }, // /
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // ==
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // !=
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // <
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // >
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // <=
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // >=
    { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '<', '<', '>', '<', '>' }, // and
    { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '<', '>' }, // or
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '<', '>', '<', '>' }, // !
    { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', ' ' }, // (
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '>' }, // )
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>' }, // id
    { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', ' ' }  // $
};
/* 
 * Function to get the precedence index of the operator
 */
int get_precedence_index(token_type type) {
    switch (type) {
        case tok_t_plus: return 0;
        case tok_t_minus: return 1;
        case tok_t_times: return 2;
        case tok_t_divide: return 3;
        case tok_t_eq: return 4;
        case tok_t_neq: return 5;
        case tok_t_lt: return 6;
        case tok_t_gt: return 7;
        case tok_t_leq: return 8;
        case tok_t_geq: return 9;
        case tok_t_and: return 10;
        case tok_t_or: return 11;
        case tok_t_not: return 12;
        case tok_t_lpa: return 13;
        case tok_t_rpa: return 14;
        case tok_t_sym: return 15;
        case tok_t_eof: return 16;
        default: return -1;
    }
}
/* 
 * Function to get the precedence of the operator
 */
int getPrecedence(token_type stackTop, token_type input) {
    return precedence_table[get_precedence_index(stackTop)][get_precedence_index(input)];
}
void print_postfix_and_stack(char *postfix, int postfix_index, Stack *stack) {
    printf("Postfix: ");
    for (int i = 0; i < postfix_index; i++) {
        printf("%c ", postfix[i]);
    }
    printf("\nStack: ");
    for (int i = 0; i <= stack->top; i++) {
        printf("%s ", tok_type_to_str(stack->items[i]->type));
    }
    printf("\n");
}
/* 
 * Function to print postfix expression
 */
void print_postfix(Token_ptr *postfix, int postfix_index) {
    printf("Postfix expression: ");
    for (int i = 0; i < postfix_index; i++) {
        printf("%s ", tok_type_to_str(postfix[i]->type));
    }
    printf("\n");
}
/* 
 * Function to parse the expression
 */
void parse_expression() {
    printi(format[_comment], "<expression>");

    Stack stack;
    init(&stack);

    Token_ptr postfix[MAX];
    int postfix_index = 0;
    int paren_count = 0;  // Na sledovanie počtu otvorených zátvoriek
    bool expression_continue = true;
    while (current_token->type != tok_t_semicolon && current_token->type != tok_t_com && expression_continue) {
        //TODO: Implement functions for precedence parsing
        switch (current_token->type) {
        case tok_t_rpa: // )
            paren_count--;
            // Ak máme 0 zátvoriek, skončíme spracovanie
            if (paren_count <= 0) {
                expression_continue = false;
                break;  // Končíme spracovanie podmienky
            }
            // Inak pokračujeme v spracovaní výrazu
            while (!isEmpty(&stack) && peek(&stack)->type != tok_t_lpa) {
                postfix[postfix_index++] = pop(&stack);
            }
            pop(&stack); // Discard the left parenthesis
            next_token();
            break;
        
        case tok_t_lpa: // (
            paren_count++;
            push(&stack, current_token);
            break;
        case tok_t_int: //1
            if(left_data->type == DATA_TYPE_INT || left_data->type == DATA_TYPE_DOUBLE){
                left_data->init = true;
            }
            else if(left_data->type == DATA_TYPE_UND){
                left_data->type = DATA_TYPE_INT;
                left_data->init = true;
            }
            else{
                //chyba -> přiřazaní int do špatného datového typu
            }
            postfix[postfix_index++] = current_token;
            next_token();
            break;
        case tok_t_flt: //1.0
            if(left_data->type == DATA_TYPE_DOUBLE){
                left_data->init = true;
            }
            else if(left_data->type == DATA_TYPE_UND){
                left_data->type = DATA_TYPE_DOUBLE;
                left_data->init = true;
            }   
            else{
                //chyba -> přiřazaní double do špatného datového typu
            }
            postfix[postfix_index++] = current_token;
            next_token();
            break;
        case tok_t_bool: // true
            if(left_data->type == DATA_TYPE_BOOLEAN){
                left_data->init = true;
            }
            else if(left_data->type == DATA_TYPE_UND){
                left_data->type = DATA_TYPE_BOOLEAN;
                left_data->init = true;
            }
            else{
                    //chyba -> přiřazaní bool do špatného datového typu
            }
            postfix[postfix_index++] = current_token;
            next_token();
            break;
        case tok_t_null: // null
            if(left_data->canNull == false){
                //chyba -> přiřazaní null do nenullovatelné hodnoty
            }
            postfix[postfix_index++] = current_token;
            next_token();
            break;
        case tok_t_unreach: // unreachable
            postfix[postfix_index++] = current_token;
            next_token();
            break;

        case tok_t_sym: // id
            postfix[postfix_index++] = current_token;
            push(&stack_codegen, current_token);
            char destination[MAX_STRING_LEN];
            sprintf(destination, "LF@%s", current_token->attribute);

            next_token();
            id_continue(); OK;

            printi(format[_defvar], destination);
            printi(format[_move], destination, string_buffer_value);
            break;
        
        case tok_t_as: // @as
            next_token();
            expect_type(tok_t_lpa); OK;

            next_token();
            expect_type(tok_t_i32); OK;

            next_token();
            expect_type(tok_t_com); OK;

            next_token();
            expect_type(tok_t_sym); OK;
            postfix[postfix_index++] = current_token;

            next_token();
            expect_type(tok_t_rpa); OK;

            next_token();
            break;

        default:
            expect_types(15, tok_t_plus, tok_t_minus, tok_t_times, tok_t_divide, tok_t_not, tok_t_eq, tok_t_neq, tok_t_lt, tok_t_gt, tok_t_leq, tok_t_geq, tok_t_and, tok_t_or, tok_t_not, tok_t_orelse); OK;
            while (!isEmpty(&stack) && getPrecedence(peek(&stack)->type, current_token->type) != '<') {
                postfix[postfix_index++] = pop(&stack);
            }
            push(&stack, current_token);

            next_token();
            break;
        }
    }

    // Pop remaining operators
    while (!isEmpty(&stack)) {
        postfix[postfix_index++] = pop(&stack);
    }
    printi_postfix(&string_tmp, postfix, postfix_index);
    //print_postfix(postfix, postfix_index);
    printi(format[_comment], "</expression>");
}
/* 
 * Function to print the type and attribute of the current token
 */
void print_token() {
    printf("->%s : %s\n", tok_type_to_str(current_token->type), current_token->attribute ? current_token->attribute : "(null)");
}
/* 
 * Function to get the next token from the scanner
 */
void next_token_initial() {
    if (current_token != NULL) {
        tok_free(current_token);
    }
    current_token = scn_scan(scanner);
    if (current_token->type == tok_t_error) {
        tok_free(current_token);
        current_token = NULL;
        error = err_lexic;
    }
}

void next_token() {
    current_token = scn_scan(scanner);
    while (current_token->type == tok_t_doc) {
        current_token = scn_scan(scanner);
    }
}
/* 
 * Function to check if the current token is of the expected type
 */
void expect_type(token_type type) {
    if (current_token->type != type) {
        fprintf(stderr, "Syntax error1: Expected %s, got %s\n", tok_type_to_str(type), tok_type_to_str(current_token->type));
        error = err_syntax;
    }
}
/*
* Function to check if the current token is of the expected type
*/
void expect_types(int count, ...) {
    va_list args;
    va_start(args, count);
    bool found = false;
    token_type expected_types[count];
    // První průchod - kontrola shody a uložení typů
    for (int i = 0; i < count; i++) {
        token_type type = va_arg(args, token_type);
        expected_types[i] = type;
        
        if (current_token->type == type) {
            found = true;
            break;
        }
    }
    
    va_end(args);
    
    // Pokud nebyla nalezena shoda, vytvoříme chybovou hlášku
    if (!found) {
        fprintf(stderr, "Syntax error: Expected one of: ");
        // Výpis všech očekávaných typů
        for (int i = 0; i < count; i++) {
            fprintf(stderr, "%s", tok_type_to_str(expected_types[i]));
            if (i < count - 1) {
                fprintf(stderr, ", ");
            }
        }
        fprintf(stderr, ", but got %s\n", tok_type_to_str(current_token->type));
        error = err_syntax;
    }
}
/* 
 * Function to check if the current token has the expected attribute
 */
void expect_attribute(const char* attr) {
    printi("Expect_attribute: %s\n", attr);
    if (current_token->attribute == NULL) {
        fprintf(stderr, "Syntax error2: Expected %s, but current token has no attribute\n", attr);
        error = err_syntax;
    }
    if (strcmp(current_token->attribute, attr) != 0) {
        fprintf(stderr, "Syntax error3: Expected %s, got %s\n", attr, current_token->attribute);
        error = err_syntax;
    }
}
/************** Grammar functions ************** 
 *  Function to parse the <program> non-terminal
 */
void program() {
    printi(format[_comment], "<program>");

    next_token();
    prolog(); OK;

    printi("%s", format[_createframe]);
    printi(format[_call], "$main");
    printi(format[_jump], "&$main");

    next_token();
    function(); OK;

    pop(&stack_codegen);

    printi_builtin(&string);

    printi(format[_label], "&$main");

    printi(format[_comment], "</program>");

    str_unify(&string, &string_tmp);
    str_clear(&string_tmp);
}
/* 
 *  Grammar: <prolog> → "const" <ID> "=" "@import" "(" <STR> ")" ";"
 */
void prolog() {
    str_append(&string, format[_comment], "<prolog>");

    // "const"
    expect_type(tok_t_const); OK;  

    // "ID"
    next_token();
    expect_type(tok_t_sym); OK;
    push(&stack_codegen, current_token);

    // "="
    next_token();
    expect_type(tok_t_ass); OK;

    // "@import"
    next_token();
    expect_type(tok_t_import); OK; 

    // "("
    next_token();
    expect_type(tok_t_lpa); OK;

    // "STRING"
    next_token();
    expect_type(tok_t_str); OK;

    // ")"
    next_token();
    expect_type(tok_t_rpa); OK;

    // ";"
    next_token();
    expect_type(tok_t_semicolon); OK;

    // vložení všech built-in funkci do symtable
    symtable_insert_builtin(current_symtable); //chyba když false

    printi(".ifjcode24\n");

    printi(format[_comment], "</prolog>");
}
/* 
 *  Grammar: <function> → "pub" "fn" <ID> "(" <parameter> ")" <return_type> "{" <statement> "}" <function_next>
 */
void function() {
    printi(format[_comment], "<function>");
    
    current_symtable = DLL_Insert_last(&sym_list);

    expect_type(tok_t_pub); OK; // "pub"

    next_token();
    expect_type(tok_t_fn); OK; // "fn"

    next_token();
    expect_type(tok_t_sym); OK; // "ID"

    sprintf(string_buffer, "$%s", current_token->attribute);
    printi(format[_label], string_buffer);
    printi("%s", format[_pushframe]);

    str_unify(&string, &string_tmp);
    str_clear(&string_tmp);

    next_token(); // "("
    expect_type(tok_t_lpa); OK;  

    next_token();
    parameter(); OK;
    
    expect_type(tok_t_rpa); OK; // ")"
    
    next_token();
    return_type(); OK;

    next_token();
    expect_type(tok_t_lcbr); OK; // "{"

    next_token();
    printi(format[_defvar], "LF@_");
    body(); OK;

    expect_type(tok_t_rcbr); OK; // "}"

    DLL_Delete_last(&sym_list);// chyba když false - nepoužitá promněná (u každého)

    printi("%s", format[_popframe]);
    printi("%s", format[_return]);

    str_unify(&string, &string_tmp);
    str_clear(&string_tmp);
    str_clear(&string_defvar);

    next_token();
    function_next(); OK;

    printi(format[_comment], "</function>");
}
/*
 * Grammar: <function_next> → EOF | <function>
 */
void function_next() {
    printi(format[_comment], "<function_next>");

    if (current_token->type != tok_t_eof) {
        function(); OK;
    } else {
        expect_type(tok_t_eof); OK;
    }
    printi(format[_comment], "</function_next>");
}
/*
 * Grammar: <parameter> → ɛ | <ID> : <ID_type> <parameter_next>
 */
void parameter() {
    printi(format[_comment], "<parameter>");

    if (current_token->type == tok_t_sym) {
        sprintf(string_buffer, "LF@%s", current_token->attribute);
        printi(format[_defvar], string_buffer);
        printi(format[_pops], string_buffer);

        left_data = symtable_insert(current_symtable, current_token->attribute);

        next_token();
        expect_type(tok_t_colon); OK; // ":"

        next_token();
        if (current_token->type == tok_t_i32 || current_token->type == tok_t_f64 || current_token->type == tok_t_u8 || current_token->type == tok_t_bool || current_token->type == tok_t_i32_opt || current_token->type == tok_t_f64_opt || current_token->type == tok_t_u8_opt) {

            switch (current_token->type)
            {
            case tok_t_i32:
                left_data->type = DATA_TYPE_INT;
                break;
            case tok_t_f64:
                left_data->type = DATA_TYPE_DOUBLE;
                break;
            case tok_t_u8:
                left_data->type = DATA_TYPE_U8;
                break;
            case tok_t_bool:
                left_data->type = DATA_TYPE_BOOLEAN;
                break;
            case tok_t_i32_opt:
                left_data->type = DATA_TYPE_INT;
                left_data->canNull = true;
                break;
            case tok_t_f64_opt:
                left_data->type = DATA_TYPE_DOUBLE;
                left_data->canNull = true;
                break;
            case tok_t_u8_opt:
                left_data->type = DATA_TYPE_U8;
                left_data->canNull = true;
                break;
            
            default:
                break;
            }

            next_token();
            parameter_next(); OK;
        } else {
            fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
            error = err_syntax;
        }
    }
    printi(format[_comment], "</parameter>");
}
/*
 * Grammar: <parameter_next> → ɛ | "," <ID> : <ID_type> <parameter_next>
 */
void parameter_next() {
    printi(format[_comment], "<parameter_next>");

    if (current_token->type == tok_t_com) {
        next_token();
        parameter(); OK;
    }
    printi(format[_comment], "</parameter_next>");
}
/*
 * Grammar: <body> → ɛ | <statement> <body>
 */
void body() {
    printi(format[_comment], "<body>");
    /* Toto je správně, ale zatím to necháme takto
    while (current_token->type != tok_t_rcbr) {
        statement(); OK;
    }*/
    if (current_token->type != tok_t_rcbr) {
        statement(); OK;
        body(); OK;
    }

    printi(format[_comment], "</body>");
}
/*
 * Grammar: <statement> → <id_statement> | <declaration> | <if_statement> | <while_statement> | <for_statement> | <return_statement> | <break_statement> | <continue_statement>
 */
void statement() {
    printi(format[_comment], "<statement>");
    bool constFlag = false;

    DLL_Last(&sym_list);
    current_symtable = DLL_GetCurrent(&sym_list);

    switch (current_token->type) {
    case tok_t_unused: //toto se v podstatě nemusí generovat ani kontrolovat
        printi(format[_comment], "<id_option>");
        printi(format[_comment], "</id_option>");
        next_token();
        expect_type(tok_t_ass); OK;

        next_token();
        value(); OK;
        sprintf(string_buffer, "LF@_");
        printi(format[_move], string_buffer, string_buffer_value);

        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    case tok_t_sym:
        printi(format[_comment], "<id_option>");
        printi(format[_comment], "</id_option>");

        strcat(stringBuffer, current_token->attribute);
        push(&stack_codegen, current_token);

        next_token();
        id_statement(); OK;

        if(left_data != NULL){
            //TODO: kontrola typů
        }
        

        if(left_data->isConst){
            //chyba -> snaha přiřazení do konstanty
        }

        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    case tok_t_const:
        constFlag = true;
    case tok_t_var:
        printi(format[_comment], "<prefix>");
        printi(format[_comment], "</prefix>");

        next_token();
        expect_type(tok_t_sym); OK; // ID

        left_data = symtable_insert(current_symtable, current_token->attribute); //když null, vnitřní chyba nebo stejný název promněné v jednou bloku
        
        if(constFlag)
            left_data->isConst = true;

        sprintf(string_buffer, "LF@%s", current_token->attribute);
        printi(format[_defvar], string_buffer);
        push(&stack_codegen, current_token);
        
        next_token();
        definition(); OK;
        
        expect_type(tok_t_ass); OK;

        next_token();
        value(); OK;
        sprintf(string_buffer, "LF@%s", pop(&stack_codegen)->attribute);
        printi(format[_move], string_buffer, string_buffer_value);

        if(left_data->type == DATA_TYPE_UND){
            //TODO: Odvození typu podle výrazu
        }
        else{
            //TODO: Kontrola typů
        }

        expect_type(tok_t_semicolon); OK;
        left_data->init = true;

        next_token();
        break;

    case tok_t_if:
        next_token(); // (
        expect_type(tok_t_lpa); OK; 

        next_token();
        int if_number = counter_codegen_if++;
        sprintf(string_buffer, "LF@%%if%i", if_number);
        printi(format[_defvar], string_buffer);
        value(); OK;
        sprintf(string_buffer, "LF@%%if%i", if_number);
        printi(format[_move], string_buffer, string_buffer_value);

        expect_type(tok_t_rpa); OK;
        
        printi_condition_jump(&string_tmp, "if", if_number);

        current_symtable = DLL_Insert_last(&sym_list);

        next_token();
        sprintf(string_buffer, "LF@%%if%i", if_number);
        not_null_value(); OK;

        then(); OK;

        sprintf(string_buffer, "*$if%i", if_number);
        printi(format[_jump], string_buffer);

        sprintf(string_buffer, "!$if%i", if_number);
        printi(format[_label], string_buffer);

        DLL_Delete_last(&sym_list);

        current_symtable = DLL_Insert_last(&sym_list);
        else_then(); OK;
        DLL_Delete_last(&sym_list);

        sprintf(string_buffer, "*$if%i", if_number);
        printi(format[_label], string_buffer);
        break;

    case tok_t_while:
        int previous_cycle_while = cycle;
        counter_codegen_while += 2;
        int while_number = counter_codegen_while;
        cycle = counter_codegen_while;

        next_token();
        expect_type(tok_t_lpa); OK;

        next_token();
        sprintf(string_buffer, "LF@%%while%i", while_number);
        printi(format[_defvar], string_buffer);

        sprintf(string_buffer, "*$while%i", while_number);
        printi(format[_label], string_buffer);

        value(); OK;
        sprintf(string_buffer, "LF@%%while%i", while_number);
        printi(format[_move], string_buffer, string_buffer_value);

        expect_type(tok_t_rpa); OK;

        printi_condition_jump(&string_tmp, "while", while_number);

        current_symtable = DLL_Insert_last(&sym_list);

        next_token();
        sprintf(string_buffer, "LF@%%while%i", while_number);
        not_null_value(); OK;

        then(); OK;

        sprintf(string_buffer, "*$while%i", while_number);
        printi(format[_jump], string_buffer);

        sprintf(string_buffer, "!$while%i", while_number);
        printi(format[_label], string_buffer);
        cycle = previous_cycle_while;

        current_symtable = DLL_Insert_last(&sym_list);

        else_then(); OK;
        DLL_Delete_last(&sym_list);

        DLL_Delete_last(&sym_list);

        sprintf(string_buffer, "&$while%i", while_number);
        printi(format[_label], string_buffer);
        break;

    case tok_t_for:
        int previous_cycle_for = cycle;
        counter_codegen_for += 2;
        int for_number = counter_codegen_for;
        cycle = counter_codegen_for;

        next_token();
        expect_type(tok_t_lpa); OK;

        next_token();
        expect_type(tok_t_sym); OK;
        push(&stack_codegen, current_token);
        
        // semantic condition check for

        while(sym_list.current != sym_list.first){
            left_data = symtable_get_item(current_symtable, current_token->attribute);
            if(left_data == NULL){
                DLL_Prev(&sym_list);
                current_symtable = DLL_GetCurrent(&sym_list);
            }
            else{
                if(left_data->type != DATA_TYPE_U8 || left_data->type != DATA_TYPE_STRING){
                    // použitý špatný datový typ
                }
            }
        }

        if(sym_list.current == sym_list.first){
            // neexistující promněná
        }

        next_token();
        sprintf(string_buffer, "LF@%%forcounter%i", for_number);
        printi(format[_defvar], string_buffer);
        printi(format[_move], string_buffer, "int@0");
        sprintf(string_buffer, "LF@%%for%i", for_number);
        printi(format[_defvar], string_buffer);

        sprintf(string_buffer, "MOVE LF@%%for%i", for_number);
        id_continue(); OK;

        expect_type(tok_t_rpa); OK;

        current_symtable = DLL_Insert_last(&sym_list);
       
        next_token();
        expect_type(tok_t_alias); OK;
 
        next_token();
        expect_types(2, tok_t_sym, tok_t_unused); OK;
        printi(format[_comment], "<id_option>");
        printi(format[_comment], "</id_option>");
        char name[MAX_STRING_LEN - 5] = "_";
        if (current_token->type == tok_t_sym) {
            strcpy(name, current_token->attribute);
        }

        sprintf(string_buffer, "LF@%s", name);
        if (current_token->type == tok_t_sym) {
            printi(format[_defvar], string_buffer);
        }

        sprintf(string_buffer, "*$for%i", for_number);
        printi(format[_label], string_buffer);

        printi("%s", format[_createframe]);
        sprintf(string_buffer, "LF@%%forcounter%i", for_number);
        printi(format[_pushs], string_buffer);
        sprintf(string_buffer, "LF@%%for%i", for_number);
        printi(format[_pushs], string_buffer);
        printi(format[_add], string_buffer, string_buffer, "int@1");
        printi(format[_call], "$$$ord");
        sprintf(string_buffer, "LF@%s", name);
        printi(format[_move], string_buffer, "TF@%retval");

        printi("JUMPIFEQ !$for%i %s nil@nil\n", for_number, string_buffer);

        left_data = symtable_insert(current_symtable, current_token->attribute);
        left_data->type = DATA_TYPE_INT;
        left_data->init = true;

        next_token();
        expect_type(tok_t_alias); OK;

        next_token();

        then(); OK;

        DLL_Delete_last(&sym_list);

        sprintf(string_buffer, "*$for%i", for_number);
        printi(format[_jump], string_buffer);

        sprintf(string_buffer, "!$for%i", for_number);
        printi(format[_label], string_buffer);

        cycle = previous_cycle_for;

        DLL_Delete_last(&sym_list);
        break;

    case tok_t_return:
        next_token();
        return_value(); OK;

        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    case tok_t_break:
        if (cycle == -1) {
            error = err_syntax;
            fprintf(stderr, "Syntax error: Break statement outside of a loop\n");
            return;
        } else if (cycle % 2 == 0) {
            sprintf(string_buffer, "&$while%i", cycle);
            printi(format[_jump], string_buffer);
        } else {
            sprintf(string_buffer, "&$for%i", cycle);
            printi(format[_jump], string_buffer);
        }

        next_token();
        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    case tok_t_continue:
        if (cycle == -1) {
            error = err_syntax;
            fprintf(stderr, "Syntax error: Continue statement outside of a loop\n");
            return;
        } else if (cycle % 2 == 0) {
            sprintf(string_buffer, "*$while%i", cycle);
            printi(format[_jump], string_buffer);
        } else {
            sprintf(string_buffer, "*$for%i", cycle);
            printi(format[_jump], string_buffer);
        }

        next_token();
        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    default:
        fprintf(stderr, "Syntax error: Expected statement, got %s\n", tok_type_to_str(current_token->type));
        error = err_syntax;
    }

    printi(format[_comment], "</statement>");
}

void id_statement() {
    printi(format[_comment], "<id_statement>");

    if (current_token->type == tok_t_ass) {
        while(sym_list.current != sym_list.first){
            left_data = symtable_get_item(current_symtable, stringBuffer);
            if (left_data == NULL){        
                DLL_Prev(&sym_list);
                current_symtable = DLL_GetCurrent(&sym_list);
            }
            else{
                break;
            } 
        }

        if(sym_list.current == sym_list.first){
            // neexistující promněná
        }

        next_token();
        stringBuffer[0] = '\0';
        value(); OK;

        sprintf(string_buffer, "LF@%s", pop(&stack_codegen)->attribute);
        printi(format[_move], string_buffer, string_buffer_value);
    } else {
        left_data = NULL;
        call(); OK;

        next_token();
    }

    printi(format[_comment], "</id_statement>");
}
/*
 * Grammar: <value> → <expression> | <literal> | <ID>
 */
void value() {
    printi(format[_comment], "<value>");

    expect_types(6, tok_t_null, tok_t_int, tok_t_flt, tok_t_bool, tok_t_as, tok_t_sym); OK;

    parse_expression(); OK;
    sprintf(string_buffer_value, "LF@%%expression%i", counter_codegen_expression++);
    printi(format[_defvar], string_buffer_value);
    printi(format[_pops], string_buffer_value);

    printi(format[_comment], "</value>");
}
/*
 * Grammar: <not_null_value> → <ID> <id_continue> | <STRING> | <value>
 */
void not_null_value() {
    printi(format[_comment], "<not_null_value>");

    if (current_token->type == tok_t_alias) {
        next_token();
        expect_types(2, tok_t_sym, tok_t_unused); OK; // ID
        char source[MAX_STRING_LEN];
        strcpy(source, string_buffer);
        if (current_token->type == tok_t_sym) {
            sprintf(string_buffer, "LF@%s", current_token->attribute);
            printi(format[_defvar], string_buffer);
        } else {
            sprintf(string_buffer, "LF@_");
        }
        printi(format[_move], string_buffer, source);

        next_token();
        expect_type(tok_t_alias); OK; // |

        next_token();
    }

    printi(format[_comment], "</not_null_value>");
}
void then() {
    printi(format[_comment], "<then>");

    if (current_token->type == tok_t_lcbr) {
        next_token();
        body(); OK;

        expect_type(tok_t_rcbr); OK;

        next_token();
    } else {
        expect_types(10, tok_t_sym, tok_t_unused, tok_t_const, tok_t_var, tok_t_if, tok_t_while, tok_t_for, tok_t_return, tok_t_break, tok_t_continue); OK;
        statement(); OK;
    }

    printi(format[_comment], "</then>");
}
/*
 * Grammar: <else_then> → ɛ | "else" <then>
 */
void else_then() {
    printi(format[_comment], "<else_then>");

    if (current_token->type == tok_t_else) {
        next_token();
        then(); OK;
    }
    printi(format[_comment], "</else_then>");
}
/*
 * Grammar: <id_continue> → "." <ID> <call> | "(" <call_params> ")"
 */
void id_continue() {
    printi(format[_comment], "<id_continue>");

    if (current_token->type == tok_t_dot || current_token->type == tok_t_lpa) {
        char source[MAX_STRING_LEN]; 
        strcpy(source, string_buffer_value);
        call(); OK;
        strcpy(string_buffer_value, source);

        sprintf(string_buffer_value, "TF@%%retval");

        next_token();
    } else {
        sprintf(string_buffer_value, "LF@%s", pop(&stack_codegen)->attribute);
    }

    printi(format[_comment], "</id_continue>");
}
/*
 * Grammar: <return_value> → ɛ | <value>
 */
void return_value() {
    printi(format[_comment], "<return_value>");

    if (current_token->type != tok_t_semicolon) {
        value(); OK;
        sprintf(string_buffer, "LF@%%retval");
        printi(format[_move], string_buffer, string_buffer_value);
    }

    printi(format[_comment], "</return_value>");
}
/*
 * Grammar: <call> → "." <ID> <call_params> | "(" <call_params> ")"
 */
void call() {
    printi(format[_comment], "<call>");
    
    printi("%s", format[_createframe]);
    if (current_token->type == tok_t_dot) { //a.a()
        pop(&stack_codegen);

        strcat(stringBuffer, ".");
        next_token();
        expect_type(tok_t_sym); OK;

        strcat(stringBuffer, current_token->attribute);
        current_symtable = DLL_GetFirst(&sym_list);
        right_data = symtable_get_item(current_symtable, stringBuffer);
        current_symtable = DLL_GetLast(&sym_list);
        if(right_data == NULL){
            // chyba funkce neexistuje
        }

        push(&stack_codegen, current_token);

        next_token();
        expect_type(tok_t_lpa); OK;

        next_token();
        call_params(); OK;

        expect_type(tok_t_rpa); OK;

        sprintf(string_buffer, "$$$%s", pop(&stack_codegen)->attribute);
    } else { //a()
        expect_type(tok_t_lpa); OK;

        current_symtable = DLL_GetFirst(&sym_list);
        right_data = symtable_get_item(current_symtable, stringBuffer);
        current_symtable = DLL_GetLast(&sym_list);
        if(right_data == NULL){
            // chyba funkce neexistuje
        }

        next_token();
        call_params(); OK;

        expect_type(tok_t_rpa); OK;

        sprintf(string_buffer, "$%s", pop(&stack_codegen)->attribute);
    }
    printi(format[_call], string_buffer);

    stringBuffer[0] = '\0';
    printi(format[_comment], "</call>");
}
/*
 * Grammar: <call_params> → ɛ | <call_value> <call_params_next>
 */
void call_params() {
    printi(format[_comment], "<call_params>");

    if (current_token->type != tok_t_rpa) {
        call_value(); OK;

        char source[MAX_STRING_LEN];
        strcpy(source, string_buffer_value);
        bool string_value = false;
        if (peek(&stack_codegen)->type == tok_t_str) {
            pop(&stack_codegen);
            string_value = true;
        } else if (strstr(source, "TF@%retval") != NULL) {
            sprintf(source, "GF@%%%i", counter_global++);
            printi(format[_defvar], source);
            printi(format[_move], source, "TF@%retval");
            printi("%s" ,format[_createframe]);
        }

        call_params_next(); OK;
        if (string_value) {
            printi("PUSHS string@");
            printi_string(&string_tmp, source);
            printi("\n");
        } else {
            printi(format[_pushs], source);
            printi("%s", format[_createframe]);
        }
    }

    printi(format[_comment], "</call_params>");
}
/*
 * Grammar: <call_value> → <value> | <ID> <call_params> | <STRING> <call_params>
 */
void call_value() {
    printi(format[_comment], "<call_value>");

    if (current_token->type == tok_t_str) {
        strcpy(string_buffer_value, current_token->attribute);
        push(&stack_codegen, current_token);

        DymString_Insert_Char(right_data->parameters, 's');

        next_token();
    } else {
        value(); OK;
    }

    printi(format[_comment], "</call_value>");
}
/*
 * Grammar: <call_params_next> → ɛ | "," <call_params>
 */
void call_params_next() {
    printi(format[_comment], "<call_params_next>");

    if (current_token->type == tok_t_com) {
        next_token();
        call_params(); OK;
    }
    printi(format[_comment], "</call_params_next>");
}

void return_type() {
    printi(format[_comment], "<return_type>");

    if (current_token->type != tok_t_void) {
        type(); OK;
        
        printi(format[_defvar], "LF@%retval");
    }

    printi(format[_comment], "</return_type>");
}
void type() {
    printi(format[_comment], "<type>");

    expect_types(7, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;
    
    switch (current_token->type)    // přiřazení typu funkce
                {
                case tok_t_i32:
                    left_data->type = DATA_TYPE_INT;
                    break;

                case tok_t_f64:
                    left_data->type = DATA_TYPE_DOUBLE;
                    break;

                case tok_t_u8:
                    left_data->type = DATA_TYPE_U8;
                    break;

                case tok_t_bool:
                    left_data->type = DATA_TYPE_BOOLEAN;
                    break;
                
                case tok_t_i32_opt:
                    left_data->type = DATA_TYPE_INT;
                    left_data->canNull = true;
                    break;

                case tok_t_f64_opt:
                    left_data->type = DATA_TYPE_DOUBLE;
                    left_data->canNull = true;
                    break;

                case tok_t_u8_opt:
                    left_data->type = DATA_TYPE_U8;
                    left_data->canNull = true;
                    break;
                
                default:    //nemůže nastat kdyžtak chyba nevjem
                    break;
                }

    printi(format[_comment], "</type>");
}

void definition() {
    printi(format[_comment], "<definition>");

    if (current_token->type == tok_t_colon) {
        next_token();
        type(); OK;

        next_token();
    }

    printi(format[_comment], "</definition>");
}

/*
 * Function to parse code for the first time
 */
void parse_fn_first() {
    // Get the first token.
    next_token_initial(); OK;
    // Iterate through program
    while (current_token->type != tok_t_eof) {
        if (current_token->type == tok_t_fn) {
            next_token_initial(); OK;
            expect_type(tok_t_sym); OK;

            left_data = symtable_insert(current_symtable, current_token->attribute); // když vrátí NULL chyba
            
            next_token_initial(); OK;
            expect_type(tok_t_lpa); OK; // (

            next_token_initial(); OK;
            if (current_token->type != tok_t_rpa) { //)   ???
                expect_type(tok_t_sym); OK;

                next_token_initial(); OK;
                expect_type(tok_t_colon); OK;

                next_token_initial(); OK;
                expect_types(7, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;

                symtable_insert_params(left_data, current_token->type); //když false chyba
                
                next_token_initial(); OK;
            }
            
            while (current_token->type != tok_t_rpa) { // ) ???
                expect_type(tok_t_com); OK;

                next_token_initial(); OK;
                expect_type(tok_t_sym); OK;

                next_token_initial(); OK;
                expect_type(tok_t_colon); OK;

                next_token_initial(); OK;
                expect_types(7, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;

                symtable_insert_params(left_data, current_token->type); // když false chyba

                next_token_initial(); OK;
            }
            
            next_token_initial(); OK;
            expect_types(8, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_void, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;

            switch (current_token->type)    // přiřazení typu funkce
                {
                case tok_t_i32:
                    left_data->type = DATA_TYPE_INT;
                    break;

                case tok_t_f64:
                    left_data->type = DATA_TYPE_DOUBLE;
                    break;

                case tok_t_u8:
                    left_data->type = DATA_TYPE_U8;
                    break;

                case tok_t_bool:
                    left_data->type = DATA_TYPE_BOOLEAN;
                    break;
                
                case tok_t_i32_opt:
                    left_data->type = DATA_TYPE_INT;
                    left_data->canNull = true;
                    break;

                case tok_t_f64_opt:
                    left_data->type = DATA_TYPE_DOUBLE;
                    left_data->canNull = true;
                    break;

                case tok_t_u8_opt:
                    left_data->type = DATA_TYPE_U8;
                    left_data->canNull = true;
                    break;

                case tok_t_void:
                    left_data->type = DATA_TYPE_VOID;
                    break;
                
                default:    //nemůže nastat kdyžtak chyba nevjem
                    break;
                }
        }
        next_token_initial(); OK;
    }
    // set next token for second parse
    tok_free(current_token);
}


/******** End of grammar functions ******** 
 * Function to parse the source code
 */
void parse() {
    // init dll and symtable
    DLL_Init(&sym_list);
    current_symtable = DLL_Insert_last(&sym_list);

    // Parse the source code for the first time.
    parse_fn_first();

    if (error == err_none) {
        // Initialize the stack for code generation
        init(&stack_codegen);
        str_init(&string);
        str_init(&string_tmp);
        str_init(&string_defvar);

        // Start parsing second time from the <program> non-terminal.
        program();

        if (error == err_none) {
            str_printout(&string);
        }

        str_destroy(&string);
        str_destroy(&string_tmp);
        str_destroy(&string_defvar);
    }
    DLL_Destroy(&sym_list);
}