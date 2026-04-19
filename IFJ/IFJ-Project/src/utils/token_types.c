/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#include "token_types.h"
#include <stddef.h>
#include <string.h>

Keyword keywords[] = {
    {tok_t_const, "const"},
    {tok_t_else, "else"},
    {tok_t_fn, "fn"},
    {tok_t_if, "if"},
    {tok_t_i32, "i32"},
    {tok_t_f64, "f64"},
    {tok_t_null, "null"},
    {tok_t_pub, "pub"},
    {tok_t_return, "return"},
    {tok_t_u8, "[]u8"},
    {tok_t_var, "var"},
    {tok_t_void, "void"},
    {tok_t_while, "while"},
    {tok_t_i32_opt, "?i32"},
    {tok_t_f64_opt, "?f64"},
    {tok_t_u8_opt, "?[]u8"},
    {tok_t_import, "@import"},
    {tok_t_as, "@as"},
    {tok_t_for, "for"},
    {tok_t_break, "break"},
    {tok_t_continue, "continue"},
    {tok_t_bool, "bool"},
    {tok_t_or, "or"},
    {tok_t_and, "and"},
    {tok_t_orelse, "orelse"},
    {tok_t_unreach, "unreachable"},
    {tok_t_true, "true"},
    {tok_t_false, "false"},
};

/**
 * @brief Vraci lexem dle typu tokenu.
 * @param type typ tokenu
 * @return lexem tokenu nebo NULL, pokud nebyl nalezen
 */
char *kw_get_lexeme(token_type type)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(Keyword); i++)
    {
        if (keywords[i].type == type)
        {
            return keywords[i].lexeme;
        }
    }
    return NULL;
}

/**
 * @brief Najde typ tokenu odpovidajici danemu lexemu.
 * @param lexeme lexem, pro ktery se hleda typ tokenu
 * @return typ tokenu, nebo tok_t_sym, pokud nebyl nalezen
 */
token_type kw_get_type(char *lexeme)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(Keyword); i++)
    {
        if (strcmp(keywords[i].lexeme, lexeme) == 0)
        {
            return keywords[i].type;
        }
    }
    return tok_t_sym;
}

char *tok_type_to_str(token_type type)
{
    Keyword all_tokens[] = {
        {tok_t_init, "tok_t_init"},
        {tok_t_sym, "tok_t_sym"},
        {tok_t_nl, "tok_t_nl"},
        {tok_t_const, "tok_t_const"},
        {tok_t_else, "tok_t_else"},
        {tok_t_fn, "tok_t_fn"},
        {tok_t_if, "tok_t_if"},
        {tok_t_i32, "tok_t_i32"},
        {tok_t_f64, "tok_t_f64"},
        {tok_t_i32_opt, "tok_t_i32_opt"},
        {tok_t_f64_opt, "tok_t_f64_opt"},
        {tok_t_null, "tok_t_null"},
        {tok_t_pub, "tok_t_pub"},
        {tok_t_return, "tok_t_return"},
        {tok_t_u8, "tok_t_u8"},
        {tok_t_u8_opt, "tok_t_u8_opt"},
        {tok_t_var, "tok_t_var"},
        {tok_t_void, "tok_t_void"},
        {tok_t_while, "tok_t_while"},
        {tok_t_for, "tok_t_for"},
        {tok_t_break, "tok_t_break"},
        {tok_t_continue, "tok_t_continue"},
        {tok_t_import, "tok_t_import"},
        {tok_t_as, "tok_t_as"},
        {tok_t_fnbui, "tok_t_fnbui"},
        {tok_t_unused, "tok_t_unused"},
        {tok_t_flt, "tok_t_flt"},
        {tok_t_int, "tok_t_int"},
        {tok_t_str, "tok_t_str"},
        {tok_t_mstr, "tok_t_mstr"},
        {tok_t_lbr, "tok_t_lbr"},
        {tok_t_rbr, "tok_t_rbr"},
        {tok_t_lcbr, "tok_t_lcbr"},
        {tok_t_rcbr, "tok_t_rcbr"},
        {tok_t_lpa, "tok_t_lpa"},
        {tok_t_rpa, "tok_t_rpa"},
        {tok_t_ass, "tok_t_ass"},
        {tok_t_plus, "tok_t_plus"},
        {tok_t_minus, "tok_t_minus"},
        {tok_t_times, "tok_t_times"},
        {tok_t_divide, "tok_t_divide"},
        {tok_t_not, "tok_t_not"},
        {tok_t_neq, "tok_t_neq"},
        {tok_t_eq, "tok_t_eq"},
        {tok_t_lt, "tok_t_lt"},
        {tok_t_gt, "tok_t_gt"},
        {tok_t_leq, "tok_t_leq"},
        {tok_t_geq, "tok_t_geq"},
        {tok_t_doc, "tok_t_doc"},
        {tok_t_dot, "tok_t_dot"},
        {tok_t_hashtag, "tok_t_hashtag"},
        {tok_t_quot, "tok_t_quot"},
        {tok_t_colon, "tok_t_colon"},
        {tok_t_semicolon, "tok_t_semicolon"},
        {tok_t_com, "tok_t_com"},
        {tok_t_eof, "tok_t_eof"},
        {tok_t_alias, "tok_t_alias"},
        {tok_t_bool, "tok_t_bool"},
        {tok_t_or, "tok_t_or"},
        {tok_t_and, "tok_t_and"},
        {tok_t_orelse, "tok_t_orelse"},
        {tok_t_unreach, "tok_t_unreach"},
        {tok_t_error, "tok_t_error"},
        {tok_t_true, "tok_t_true"},
        {tok_t_false, "tok_t_false"}
        };
    for (int i = 0; i < sizeof(all_tokens) / sizeof(Keyword); i++)
    {
        if (all_tokens[i].type == type)
        {
            return all_tokens[i].lexeme;
        }
    }
    return NULL;
}