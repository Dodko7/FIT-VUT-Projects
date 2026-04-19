/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#ifndef KEYWORDS_H
#define KEYWORDS_H

/**
 * @brief Typ tokenu
 *
 * Enum který reprezentuje dosažitelné typy tokenů
 */
typedef enum _token_type
{
	tok_t_init,		// initial token value, used only for initialization
	tok_t_sym,		// symbol
	tok_t_nl,		// newline
	tok_t_const,	// constant
	tok_t_else,		// else
	tok_t_fn,		// function
	tok_t_if,		// if
	tok_t_i32,		// int32
	tok_t_f64,		// float64
	tok_t_i32_opt,	// int32 optional
	tok_t_f64_opt,	// float64 optional
	tok_t_null,		// null
	tok_t_pub,		// public
	tok_t_return,	// return
	tok_t_u8,		// uint8, string
	tok_t_u8_opt,	// uint8, string optional
	tok_t_var,		// variable
	tok_t_void,		// void
	tok_t_while,	// while
	tok_t_for,		// for (extension)
	tok_t_break,	// break (extension)
	tok_t_continue, // continue (extension)
	tok_t_import,	// @import
	tok_t_as,		// @as
	tok_t_fnbui,	// builtin fn
	// https://ziggit.dev/t/trying-to-understand-what-an-underscore-before-assignment-means/4611
	tok_t_unused, // underscore, according to the internet it should mark as unused

	tok_t_flt,	  // DOUBLE or floating point value
	tok_t_int,	  // INT or integer value
	tok_t_str,	  // STRING or anything enclosed in quotes
	tok_t_mstr,	  // MULTILINE STRING
	tok_t_lbr,	  // [, left brace
	tok_t_rbr,	  // ], right brace
	tok_t_lcbr,	  // {, left curly brace
	tok_t_rcbr,	  // }, right curly brace
	tok_t_lpa,	  // (, left parenthese
	tok_t_rpa,	  // ), right parenthese
	tok_t_ass,	  // =, assignment operator
	tok_t_plus,	  // +, plus operator
	tok_t_minus,  // -, minus operator
	tok_t_times,  // *, times operator
	tok_t_divide, // /, divide operator

	tok_t_not, // !, not
	tok_t_neq, // !=, not equal
	tok_t_eq,  // ==, equal
	tok_t_lt,  // <, less than
	tok_t_gt,  // >, greater than
	tok_t_leq, // <=, less or equal
	tok_t_geq, // >=, greater or equal

	tok_t_doc, // comment

	tok_t_dot,		 // . dot
	tok_t_hashtag,	 // #
	tok_t_quot,		 // quote
	tok_t_colon,	 // :
	tok_t_semicolon, // ;
	tok_t_com,		 // comma
	tok_t_eof,		 // end of file
	tok_t_alias,	 // alias (those |inpOk| things around)
	tok_t_bool,		 // bool (extension)
	tok_t_or,		 // or (extension)
	tok_t_and,		 // and (extension)
	tok_t_orelse,	 // or else (extension)
	tok_t_orelse_un, // or else unreachable (extension)
	tok_t_unreach,	 // unreachable (extension)
	tok_t_error,	 // throws that the error state has been set
	tok_t_true,		 // throws that the error state has been set
	tok_t_false,	 // throws that the error state has been set
} token_type;

typedef struct _Keyword
{
	token_type type;
	char *lexeme;
} Keyword;

char *kw_get_lexeme(token_type type);
char *tok_type_to_str(token_type type);
token_type kw_get_type(char *lexeme);
extern Keyword keywords[];
#endif // KEYWORDS_H