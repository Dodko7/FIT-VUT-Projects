#ifndef TOKEN_H
#define TOKEN_H
#include "token_types.h"

/**
 * @struct _Token
 * @brief Struktura reprezentující token
 *
 * Struktura _Token reprezentuje jeden token, který  je
 * výstupem lexikální analýzy. Token má atribut type, který udává typ
 * tokenu, a atribut attribute, který udává hodnotu tokenu.
 */
typedef struct _Token
{
    token_type type; /** < Typ tokenu */
    char *attribute; /** < Hodnota tokenu */
} Token;

typedef struct _Token *Token_ptr;

Token_ptr tok_init(token_type type);

void tok_init_no_ret(Token_ptr* tok);

Token_ptr tok_init_no_type();

void tok_copy(Token_ptr dst, Token tok);

void tok_free(Token_ptr tok);

void tok_print(Token_ptr tok);

token_type tok_get_type(Token_ptr tok);

char *tok_get_attribute(Token_ptr tok);

void tok_set_attribute(Token_ptr tok, char *attribute);

void tok_set_type(Token_ptr tok, token_type type);

void tok_set_type_attribute(Token_ptr tok, token_type type, char *attribute);
#endif // TOKEN_H