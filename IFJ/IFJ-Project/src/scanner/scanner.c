/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "scanner.h"
#include "scan_state.h"
#include "../utils/memory_table.h"
#include "../utils/errors.h"

SCA_MATCH_DECL(underscore, '_')
SCA_MATCH_DECL(question_mark, '?')
SCA_MATCH_DECL(brace_open, '[')
SCA_MATCH_DECL(brace_close, ']')
SCA_MATCH_DECL(at, '@')
SCA_MATCH_DECL(dot, '.')
SCA_MATCH_DECL(comma, ',')
SCA_MATCH_DECL(quote, '"')
SCA_MATCH_DECL(backslash, '\\')
SCA_MATCH_DECL(eof, EOF)
SCA_MATCH_DECL(end, '\0')
SCA_MATCH_DECL(curlybrace_close, '}')
SCA_MATCH_DECL(curlybrace_open, '{')
SCA_MATCH_DECL(parenthese_open, '(')
SCA_MATCH_DECL(parenthese_close, ')')
SCA_MATCH_DECL(semicolon, ';')
SCA_MATCH_DECL(slash, '/')
SCA_MATCH_DECL(vertical, '|')
SCA_MATCH_DECL(colon, ':')
SCA_MATCH_DECL(exclamation, '!')
SCA_MATCH_DECL(greater, '>')
SCA_MATCH_DECL(equal, '=')
SCA_MATCH_DECL(less, '<')
SCA_MATCH_DECL(asterisk, '*')
SCA_MATCH_DECL(minus, '-')
SCA_MATCH_DECL(plus, '+')
SCA_MATCH_DECL(u, 'u')
SCA_MATCH_DECL(x, 'x')
SCA_MATCH_DECL(E, 'E')
SCA_MATCH_DECL(e, 'e')
SCA_MATCH_DECL(hashtag, '#')
SCA_MATCH_DECL(newline, '\n')
SCA_MATCH_DECL(r, 'r')
SCA_MATCH_DECL(n, 'n')
SCA_MATCH_DECL(t, 't')
SCA_GREATER_DECL(text, 34)
SCA_GREATER_DECL(comments, 31) // for comments

/**
 * @brief Podmínka pro řetězce. Kontroluje, zda znak c je tisknutelný a není zpětným lomítkem či uvozovkou.
 *
 * @param c Znak, který má být zkontrolován.
 * @return Vrací nenulovou hodnotu, pokud je znak tisknutelný a není zpětným lomítkem či uvozovkou, jinak vrací 0.
 */
int sca_string(int c)
{
	return c != '\\' && c != '"' && isprint(c);
}
// initial node for all tokens
Scan_node sca_init = {.state = sca_s_init, .children = NULL, .count = 0};

Scan_node sca_underscore = {.state = sca_s_underscore, .children = NULL, .count = 0};
Scan_node sca_lexeme = {.state = sca_s_lexeme, .children = NULL, .count = 0};
Scan_node sca_opt_str_lexeme = {.state = sca_s_opt_str_lexeme, .children = NULL, .count = 0}; // optional or string lexeme

Scan_node sca_qm = {.state = sca_s_qm, .children = NULL, .count = 0};
Scan_node sca_bro = {.state = sca_s_bro, .children = NULL, .count = 0};
Scan_node sca_brc = {.state = sca_s_brc, .children = NULL, .count = 0};

Scan_node sca_at = {.state = sca_s_at, .children = NULL, .count = 0};
Scan_node sca_atid = {.state = sca_s_atid, .children = NULL, .count = 0};

Scan_node sca_dt = {.state = sca_s_dt, .children = NULL, .count = 0};
Scan_node sca_orelse_op = {.state = sca_s_orelse_op, .children = NULL, .count = 0};

Scan_node sca_int = {.state = sca_s_int, .children = NULL, .count = 0};
Scan_node sca_intdt = {.state = sca_s_intdt, .children = NULL, .count = 0};
Scan_node sca_intexpsign = {.state = sca_s_intexpsign, .children = NULL, .count = 0};
Scan_node sca_intexp = {.state = sca_s_intexp, .children = NULL, .count = 0};
Scan_node sca_dec = {.state = sca_s_dec, .children = NULL, .count = 0};
Scan_node sca_dec2 = {.state = sca_s_dec2, .children = NULL, .count = 0};

Scan_node sca_s1 = {.state = sca_s_s1, .children = NULL, .count = 0};
Scan_node sca_s2 = {.state = sca_s_s2, .children = NULL, .count = 0};
Scan_node sca_s3 = {.state = sca_s_s3, .children = NULL, .count = 0};
Scan_node sca_s_max1 = {.state = sca_s_s_max1, .children = NULL, .count = 0};
Scan_node sca_s_max2 = {.state = sca_s_s_max2, .children = NULL, .count = 0};
Scan_node sca_s5 = {.state = sca_s_s5, .children = NULL, .count = 0};
Scan_node sca_str = {.state = sca_s_str, .children = NULL, .count = 0};

Scan_node sca_backslash = {.state = sca_s_backslash, .children = NULL, .count = 0};
Scan_node sca_ml1 = {.state = sca_s_ml1, .children = NULL, .count = 0};
Scan_node sca_ml2 = {.state = sca_s_ml2, .children = NULL, .count = 0};
Scan_node sca_ml3 = {.state = sca_s_ml3, .children = NULL, .count = 0};
Scan_node sca_ml_max1 = {.state = sca_s_ml_max1, .children = NULL, .count = 0};
Scan_node sca_ml_max2 = {.state = sca_s_ml_max2, .children = NULL, .count = 0};
Scan_node sca_ml5 = {.state = sca_s_ml5, .children = NULL, .count = 0};
Scan_node sca_ml_str = {.state = sca_s_ml_str, .children = NULL, .count = 0};
Scan_node sca_ml_slash = {.state = sca_s_ml_slash, .children = NULL, .count = 0};
Scan_node sca_ml_comment = {.state = sca_s_ml_comment, .children = NULL, .count = 0};

Scan_node sca_eof = {.state = sca_s_eof, .children = NULL, .count = 0};
Scan_node sca_curlybrace_close = {.state = sca_s_curlybrace_close, .children = NULL, .count = 0};
Scan_node sca_curlybrace_open = {.state = sca_s_curlybrace_open, .children = NULL, .count = 0};
Scan_node sca_parenthese_open = {.state = sca_s_parenthese_open, .children = NULL, .count = 0};
Scan_node sca_parenthese_close = {.state = sca_s_parenthese_close, .children = NULL, .count = 0};
Scan_node sca_semicolon = {.state = sca_s_semicolon, .children = NULL, .count = 0};
Scan_node sca_slash = {.state = sca_s_slash, .children = NULL, .count = 0};
Scan_node sca_comment = {.state = sca_s_comment, .children = NULL, .count = 0};
Scan_node sca_vertical = {.state = sca_s_vertical, .children = NULL, .count = 0};
Scan_node sca_colon = {.state = sca_s_colon, .children = NULL, .count = 0};
Scan_node sca_exclamation = {.state = sca_s_exclamation, .children = NULL, .count = 0};
Scan_node sca_notequal = {.state = sca_s_notequal, .children = NULL, .count = 0};
Scan_node sca_greater = {.state = sca_s_greater, .children = NULL, .count = 0};
Scan_node sca_greaterequal = {.state = sca_s_greaterequal, .children = NULL, .count = 0};
Scan_node sca_assign = {.state = sca_s_assign, .children = NULL, .count = 0};
Scan_node sca_equal = {.state = sca_s_equal, .children = NULL, .count = 0};
Scan_node sca_less = {.state = sca_s_less, .children = NULL, .count = 0};
Scan_node sca_lessequal = {.state = sca_s_lessequal, .children = NULL, .count = 0};
Scan_node sca_asterisk = {.state = sca_s_asterisk, .children = NULL, .count = 0};
Scan_node sca_minus = {.state = sca_s_minus, .children = NULL, .count = 0};
Scan_node sca_plus = {.state = sca_s_plus, .children = NULL, .count = 0};
Scan_node sca_comma = {.state = sca_s_comma, .children = NULL, .count = 0};
Scan_node sca_hashtag = {.state = sca_s_hashtag, .children = NULL, .count = 0};

// init to init
SCA_PATH_DEF(sca_init, sca_init)

// init to lexeme identification paths
SCA_PATH_DEF(sca_init, sca_underscore)
SCA_PATH_DEF(sca_init, sca_lexeme)
SCA_PATH_DEF(sca_lexeme, sca_lexeme)
SCA_PATH_DEF(sca_underscore, sca_lexeme)

// init to optional/string data type paths
SCA_PATH_DEF(sca_init, sca_qm)
SCA_PATH_DEF(sca_init, sca_bro)
SCA_PATH_DEF(sca_qm, sca_bro)
SCA_PATH_DEF(sca_bro, sca_brc)
SCA_PATH_DEF(sca_brc, sca_opt_str_lexeme)
SCA_PATH_DEF(sca_qm, sca_opt_str_lexeme)
SCA_PATH_DEF(sca_opt_str_lexeme, sca_opt_str_lexeme)

// init to import and as keywords
SCA_PATH_DEF(sca_init, sca_at)
SCA_PATH_DEF(sca_at, sca_atid)
SCA_PATH_DEF(sca_atid, sca_atid)

// init to builtin function paths
SCA_PATH_DEF(sca_init, sca_dt)
SCA_PATH_DEF(sca_dt, sca_dt)
SCA_PATH_DEF(sca_dt, sca_orelse_op)

// init to number paths
SCA_PATH_DEF(sca_init, sca_int)
SCA_PATH_DEF(sca_int, sca_int)
SCA_PATH_DEF(sca_int, sca_intexp)
SCA_PATH_DEF(sca_int, sca_intdt)

SCA_PATH_DEF(sca_intdt, sca_dec)
SCA_PATH_DEF(sca_dec, sca_dec)
SCA_PATH_DEF(sca_dec, sca_intexp)

SCA_PATH_DEF(sca_intexp, sca_dec2)
SCA_PATH_DEF(sca_intexp, sca_intexpsign)
SCA_PATH_DEF(sca_intexpsign, sca_dec2)
SCA_PATH_DEF(sca_dec2, sca_dec2)

// init to string paths
SCA_PATH_DEF(sca_init, sca_s1)
SCA_PATH_DEF(sca_s1, sca_s5)
SCA_PATH_DEF(sca_s1, sca_s2)
SCA_PATH_DEF(sca_s2, sca_s3)

SCA_PATH_DEF(sca_s3, sca_s_max1)
SCA_PATH_DEF(sca_s_max1, sca_s_max2)
SCA_PATH_DEF(sca_s_max2, sca_s2)
SCA_PATH_DEF(sca_s_max2, sca_s5)

SCA_PATH_DEF(sca_s5, sca_s5)
SCA_PATH_DEF(sca_s5, sca_s2)
SCA_PATH_DEF(sca_s2, sca_s5)
SCA_PATH_DEF(sca_s1, sca_str)

SCA_PATH_DEF(sca_s5, sca_str)
SCA_PATH_DEF(sca_s_max2, sca_str)

// init to multiline paths (reserved for future implementation)
SCA_PATH_DEF(sca_init, sca_backslash)
SCA_PATH_DEF(sca_backslash, sca_ml1)
SCA_PATH_DEF(sca_ml1, sca_ml_str)
SCA_PATH_DEF(sca_ml1, sca_ml5)

SCA_PATH_DEF(sca_ml1, sca_ml2)
SCA_PATH_DEF(sca_ml2, sca_ml3)
SCA_PATH_DEF(sca_ml2, sca_ml5)
SCA_PATH_DEF(sca_ml3, sca_ml_max1)

SCA_PATH_DEF(sca_ml_max1, sca_ml_max2)
SCA_PATH_DEF(sca_ml_max2, sca_ml2)
SCA_PATH_DEF(sca_ml_max2, sca_ml5)
SCA_PATH_DEF(sca_ml_max2, sca_ml_str)

SCA_PATH_DEF(sca_ml5, sca_ml5)
SCA_PATH_DEF(sca_ml5, sca_ml2)
SCA_PATH_DEF(sca_ml5, sca_ml_str)
SCA_PATH_DEF(sca_ml_str, sca_ml_str)
SCA_PATH_DEF(sca_ml_str, sca_ml_slash)
SCA_PATH_DEF(sca_ml_slash, sca_ml_comment)
SCA_PATH_DEF(sca_ml_comment, sca_ml_comment)
SCA_PATH_DEF(sca_ml_comment, sca_ml_str)

SCA_PATH_DEF(sca_ml_str, sca_backslash)

// init to eof
SCA_PATH_DEF(sca_init, sca_eof)

// init to curly brace paths
SCA_PATH_DEF(sca_init, sca_curlybrace_close)
SCA_PATH_DEF(sca_init, sca_curlybrace_open)

// init to parenthese paths
SCA_PATH_DEF(sca_init, sca_parenthese_close)
SCA_PATH_DEF(sca_init, sca_parenthese_open)

// init to semicolon paths
SCA_PATH_DEF(sca_init, sca_semicolon)

// init to slash paths
SCA_PATH_DEF(sca_init, sca_slash)
SCA_PATH_DEF(sca_slash, sca_comment)
SCA_PATH_DEF(sca_comment, sca_comment)

// init to vertical path
SCA_PATH_DEF(sca_init, sca_vertical)

// init to colon paths
SCA_PATH_DEF(sca_init, sca_colon)

// init to exclamation paths
SCA_PATH_DEF(sca_init, sca_exclamation)
SCA_PATH_DEF(sca_exclamation, sca_notequal)

// init to greater paths
SCA_PATH_DEF(sca_init, sca_greater)
SCA_PATH_DEF(sca_greater, sca_greaterequal)

// init to equal paths
SCA_PATH_DEF(sca_init, sca_assign)
SCA_PATH_DEF(sca_assign, sca_equal)

// init to less paths
SCA_PATH_DEF(sca_init, sca_less)
SCA_PATH_DEF(sca_less, sca_lessequal)

// init to asterisk path
SCA_PATH_DEF(sca_init, sca_asterisk)

// init to plus path
SCA_PATH_DEF(sca_init, sca_plus)

// init to minus path
SCA_PATH_DEF(sca_init, sca_minus)

// init to comma path
SCA_PATH_DEF(sca_init, sca_comma)

// init to hashtag path
SCA_PATH_DEF(sca_init, sca_hashtag)

/**
 * @brief dosadi cesty k Scan_node.
 * @details Tato funkce pouziva seznam variabilnich argumentu k pripojeni synu
 * ke Scan_node.
 * @param node Scan_node, ke ktere mame pripojit deti.
 * @param argc Pocet argumentu.
 * @param ...
 */
void sca_assign_children(Scan_node_ptr node, int argc, ...)
{
	va_list args;
	va_start(args, argc);
	node->children = imalloc(argc * sizeof(Scan_path *));
	node->count = argc;
	for (size_t i = 0; i < argc; i++)
		node->children[i] = va_arg(args, Scan_path *);
	va_end(args);
}

/**
 * @brief Uvolni pamet, kterou mel uzel skeneru.
 * @details Funkce projde vsechny cesty, ktere z uzlu vedou, uvolni
 * pamet, kterou mely. Pak uvolni pamet, kterou mel na cesty
 *
 * @param node Uzel, jehoz cesty se maji uvolnit.
 */
void sca_free(Scan_node_ptr node)
{

	for (size_t i = 0; i < node->count; i++)
	{
		SCA_PATH_DEINIT((*node->children[i]))
	}
}

/**
 * @brief Vytvori scanner z konfiguracního souboru.
 * @details Vytvori strukturu Scanner a inicializuje ji s obsahem souboru filename.
 * @param filename Cesta k souboru, ze ktereho chceme cist.
 * @return Struktura Scanner, nebo NULL, pokud doslo k chybe.
 */
Scanner_ptr scn_init(char *filename)
{
	// scanner setup
	Scanner_ptr scanner = imalloc(sizeof(Scanner));

	if (scanner == NULL)
	{
		// TODO: throw error err_internal
		exit_internal();
	}
	scanner->file_name = imalloc(strlen(filename) + 1);
	if (scanner->file_name == NULL)
	{
		// TODO: throw error err_internal
		exit_internal();
	}
	strcpy(scanner->file_name, filename);

	scanner->source = scn_open_file(scanner);
	if (scanner->source == NULL)
	{
		// TODO: throw error err_internal
		exit_internal();
	}
	scanner->source_size = strlen(scanner->source);
	scanner->source_index = 0;
	scanner->source_line = 0;

	scanner->line = 1;
	scanner->is_scanned = false;
	scanner->list = tok_dll_init();

	/** scanner graph path config */
	// init to init
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_init), isspace)

	// init to lexeme identification paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_underscore), SCA_MATCH(underscore))
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_lexeme), isalpha)
	SCA_PATH_INIT(SCA_PATH(sca_lexeme, sca_lexeme), isalnum, SCA_MATCH(underscore))
	SCA_PATH_INIT(SCA_PATH(sca_underscore, sca_lexeme), isalnum, SCA_MATCH(underscore))

	// init to optional/string data type paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_qm), SCA_MATCH(question_mark))
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_bro), SCA_MATCH(brace_open))
	SCA_PATH_INIT(SCA_PATH(sca_qm, sca_bro), SCA_MATCH(brace_open))
	SCA_PATH_INIT(SCA_PATH(sca_bro, sca_brc), SCA_MATCH(brace_close))
	SCA_PATH_INIT(SCA_PATH(sca_brc, sca_opt_str_lexeme), SCA_MATCH(u))
	SCA_PATH_INIT(SCA_PATH(sca_qm, sca_opt_str_lexeme), isalnum)
	SCA_PATH_INIT(SCA_PATH(sca_opt_str_lexeme, sca_opt_str_lexeme), isalnum)

	// init to import and as keywords
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_at), SCA_MATCH(at))
	SCA_PATH_INIT(SCA_PATH(sca_at, sca_atid), isalpha)
	SCA_PATH_INIT(SCA_PATH(sca_atid, sca_atid), isalpha)

	// init to builtin function paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_dt), SCA_MATCH(dot))
	SCA_PATH_INIT(SCA_PATH(sca_dt, sca_dt), isspace)
	SCA_PATH_INIT(SCA_PATH(sca_dt, sca_orelse_op), SCA_MATCH(question_mark))

	// init to number paths

	SCA_PATH_INIT(SCA_PATH(sca_init, sca_int), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_int, sca_int), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_int, sca_intexp), SCA_MATCH(e), SCA_MATCH(E))
	SCA_PATH_INIT(SCA_PATH(sca_int, sca_intdt), SCA_MATCH(dot))

	SCA_PATH_INIT(SCA_PATH(sca_intdt, sca_dec), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_dec, sca_dec), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_dec, sca_intexp), SCA_MATCH(e), SCA_MATCH(E))
	SCA_PATH_INIT(SCA_PATH(sca_intexp, sca_dec2), isdigit)

	SCA_PATH_INIT(SCA_PATH(sca_intexp, sca_intexpsign), SCA_MATCH(plus), SCA_MATCH(minus))
	SCA_PATH_INIT(SCA_PATH(sca_intexpsign, sca_dec2), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_dec2, sca_dec2), isdigit)

	// init to string paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_s1), SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_s1, sca_s5), sca_string)
	SCA_PATH_INIT(SCA_PATH(sca_s1, sca_s2), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_s2, sca_s3), SCA_MATCH(x))

	SCA_PATH_INIT(SCA_PATH(sca_s3, sca_s_max1), isxdigit)
	SCA_PATH_INIT(SCA_PATH(sca_s_max1, sca_s_max2), isxdigit)
	SCA_PATH_INIT(SCA_PATH(sca_s_max2, sca_s2), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_s_max2, sca_s5), sca_string)

	SCA_PATH_INIT(SCA_PATH(sca_s5, sca_s5), sca_string)
	SCA_PATH_INIT(SCA_PATH(sca_s5, sca_s2), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_s2, sca_s5), SCA_MATCH(r), SCA_MATCH(n), SCA_MATCH(t), SCA_MATCH(backslash), SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_s1, sca_str), SCA_MATCH(quote))

	SCA_PATH_INIT(SCA_PATH(sca_s5, sca_str), SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_s_max2, sca_str), SCA_MATCH(quote))

	// init to multiline paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_backslash), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_backslash, sca_ml1), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_ml1, sca_ml5), sca_string, SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_ml1, sca_ml2), SCA_MATCH(backslash))

	SCA_PATH_INIT(SCA_PATH(sca_ml1, sca_ml_str), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_ml2, sca_ml3), SCA_MATCH(x))
	SCA_PATH_INIT(SCA_PATH(sca_ml2, sca_ml5), SCA_MATCH(r), SCA_MATCH(n), SCA_MATCH(t), SCA_MATCH(backslash), SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_ml3, sca_ml_max1), isxdigit)

	SCA_PATH_INIT(SCA_PATH(sca_ml_max1, sca_ml_max2), isxdigit)
	SCA_PATH_INIT(SCA_PATH(sca_ml_max2, sca_ml2), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_ml_max2, sca_ml5), sca_string, SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_ml_max2, sca_ml_str), SCA_MATCH(newline))

	SCA_PATH_INIT(SCA_PATH(sca_ml5, sca_ml5), sca_string, SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_ml5, sca_ml2), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_ml5, sca_ml_str), SCA_MATCH(newline))
	SCA_PATH_INIT(SCA_PATH(sca_ml_str, sca_ml_str), isblank)
	SCA_PATH_INIT(SCA_PATH(sca_ml_str, sca_ml_slash), SCA_MATCH(slash))
	SCA_PATH_INIT(SCA_PATH(sca_ml_slash, sca_ml_comment), SCA_MATCH(slash))
	SCA_PATH_INIT(SCA_PATH(sca_ml_comment, sca_ml_comment), SCA_GREATER(comments))
	SCA_PATH_INIT(SCA_PATH(sca_ml_comment, sca_ml_str), SCA_MATCH(newline))

	SCA_PATH_INIT(SCA_PATH(sca_ml_str, sca_backslash), SCA_MATCH(backslash))
	// init to eof
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_eof), SCA_MATCH(eof), SCA_MATCH(end))

	// init to curly brace paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_curlybrace_close), SCA_MATCH(curlybrace_close))
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_curlybrace_open), SCA_MATCH(curlybrace_open))

	// init to parenthese paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_parenthese_close), SCA_MATCH(parenthese_close))
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_parenthese_open), SCA_MATCH(parenthese_open))

	// init to semicolon paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_semicolon), SCA_MATCH(semicolon))

	// init to slash paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_slash), SCA_MATCH(slash))
	SCA_PATH_INIT(SCA_PATH(sca_slash, sca_comment), SCA_MATCH(slash))
	SCA_PATH_INIT(SCA_PATH(sca_comment, sca_comment), SCA_GREATER(comments))

	// init to vertical path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_vertical), SCA_MATCH(vertical))

	// init to colon paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_colon), SCA_MATCH(colon))

	// init to exclamation paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_exclamation), SCA_MATCH(exclamation))
	SCA_PATH_INIT(SCA_PATH(sca_exclamation, sca_notequal), SCA_MATCH(equal))

	// init to greater paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_greater), SCA_MATCH(greater))
	SCA_PATH_INIT(SCA_PATH(sca_greater, sca_greaterequal), SCA_MATCH(equal))

	// init to equal paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_assign), SCA_MATCH(equal))
	SCA_PATH_INIT(SCA_PATH(sca_assign, sca_equal), SCA_MATCH(equal))

	// init to less paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_less), SCA_MATCH(less))
	SCA_PATH_INIT(SCA_PATH(sca_less, sca_lessequal), SCA_MATCH(equal))

	// init to asterisk path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_asterisk), SCA_MATCH(asterisk))

	// init to plus path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_plus), SCA_MATCH(plus))

	// init to minus path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_minus), SCA_MATCH(minus))

	// init to comma path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_comma), SCA_MATCH(comma))

	// init to hashtag path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_hashtag), SCA_MATCH(hashtag))

	/** scanner graph node config */
	// scanner graph node config
	sca_assign_children(&sca_init, 28, &SCA_PATH(sca_init, sca_init),
						&SCA_PATH(sca_init, sca_underscore), &SCA_PATH(sca_init, sca_lexeme), &SCA_PATH(sca_init, sca_qm),
						&SCA_PATH(sca_init, sca_bro), &SCA_PATH(sca_init, sca_at), &SCA_PATH(sca_init, sca_dt),
						&SCA_PATH(sca_init, sca_int), &SCA_PATH(sca_init, sca_s1), &SCA_PATH(sca_init, sca_backslash),
						&SCA_PATH(sca_init, sca_comma), &SCA_PATH(sca_init, sca_eof), &SCA_PATH(sca_init, sca_curlybrace_open),
						&SCA_PATH(sca_init, sca_curlybrace_close), &SCA_PATH(sca_init, sca_parenthese_open), &SCA_PATH(sca_init, sca_parenthese_close),
						&SCA_PATH(sca_init, sca_semicolon), &SCA_PATH(sca_init, sca_slash), &SCA_PATH(sca_init, sca_vertical),
						&SCA_PATH(sca_init, sca_colon), &SCA_PATH(sca_init, sca_exclamation), &SCA_PATH(sca_init, sca_greater),
						&SCA_PATH(sca_init, sca_assign), &SCA_PATH(sca_init, sca_less), &SCA_PATH(sca_init, sca_asterisk),
						&SCA_PATH(sca_init, sca_minus), &SCA_PATH(sca_init, sca_plus), &SCA_PATH(sca_init, sca_hashtag));

	sca_assign_children(&sca_underscore, 1, &SCA_PATH(sca_underscore, sca_lexeme));
	sca_assign_children(&sca_lexeme, 1, &SCA_PATH(sca_lexeme, sca_lexeme));
	sca_assign_children(&sca_opt_str_lexeme, 1, &SCA_PATH(sca_opt_str_lexeme, sca_opt_str_lexeme));

	sca_assign_children(&sca_qm, 2, &SCA_PATH(sca_qm, sca_opt_str_lexeme), &SCA_PATH(sca_qm, sca_bro));
	sca_assign_children(&sca_bro, 1, &SCA_PATH(sca_bro, sca_brc));
	sca_assign_children(&sca_brc, 1, &SCA_PATH(sca_brc, sca_opt_str_lexeme));

	sca_assign_children(&sca_at, 1, &SCA_PATH(sca_at, sca_atid));
	sca_assign_children(&sca_atid, 1, &SCA_PATH(sca_atid, sca_atid));
	// numbers

	sca_assign_children(&sca_int, 3, &SCA_PATH(sca_int, sca_intdt), &SCA_PATH(sca_int, sca_int), &SCA_PATH(sca_int, sca_intexp));
	sca_assign_children(&sca_intdt, 1, &SCA_PATH(sca_intdt, sca_dec));
	sca_assign_children(&sca_intexp, 2, &SCA_PATH(sca_intexp, sca_dec2), &SCA_PATH(sca_intexp, sca_intexpsign));
	sca_assign_children(&sca_intexpsign, 1, &SCA_PATH(sca_intexpsign, sca_dec2));
	sca_assign_children(&sca_dec, 2, &SCA_PATH(sca_dec, sca_dec), &SCA_PATH(sca_dec, sca_intexp));
	sca_assign_children(&sca_dec2, 1, &SCA_PATH(sca_dec2, sca_dec2));

	// strings
	sca_assign_children(&sca_s1, 3, &SCA_PATH(sca_s1, sca_s5), &SCA_PATH(sca_s1, sca_s2), &SCA_PATH(sca_s1, sca_str));
	sca_assign_children(&sca_s2, 2, &SCA_PATH(sca_s2, sca_s3), &SCA_PATH(sca_s2, sca_s5));
	sca_assign_children(&sca_s3, 1, &SCA_PATH(sca_s3, sca_s_max1));
	sca_assign_children(&sca_s_max1, 1, &SCA_PATH(sca_s_max1, sca_s_max2));
	sca_assign_children(&sca_s_max2, 3, &SCA_PATH(sca_s_max2, sca_s5), &SCA_PATH(sca_s_max2, sca_s2), &SCA_PATH(sca_s_max2, sca_str));
	sca_assign_children(&sca_s5, 3, &SCA_PATH(sca_s5, sca_s5), &SCA_PATH(sca_s5, sca_s2), &SCA_PATH(sca_s5, sca_str));

	// Multiline string
	sca_assign_children(&sca_backslash, 1, &SCA_PATH(sca_backslash, sca_ml1));
	sca_assign_children(&sca_ml1, 3, &SCA_PATH(sca_ml1, sca_ml5), &SCA_PATH(sca_ml1, sca_ml2), &SCA_PATH(sca_ml1, sca_ml_str));
	sca_assign_children(&sca_ml2, 2, &SCA_PATH(sca_ml2, sca_ml3), &SCA_PATH(sca_ml2, sca_ml5));
	sca_assign_children(&sca_ml3, 1, &SCA_PATH(sca_ml3, sca_ml_max1));
	sca_assign_children(&sca_ml_max1, 1, &SCA_PATH(sca_ml_max1, sca_ml_max2));
	sca_assign_children(&sca_ml_max2, 3, &SCA_PATH(sca_ml_max2, sca_ml2), &SCA_PATH(sca_ml_max2, sca_ml5), &SCA_PATH(sca_ml_max2, sca_ml_str));
	sca_assign_children(&sca_ml5, 3, &SCA_PATH(sca_ml5, sca_ml5), &SCA_PATH(sca_ml5, sca_ml2), &SCA_PATH(sca_ml5, sca_ml_str));
	sca_assign_children(&sca_ml_str, 2, &SCA_PATH(sca_ml_str, sca_ml_str), &SCA_PATH(sca_ml_str, sca_backslash));
	sca_assign_children(&sca_ml_str, 3, &SCA_PATH(sca_ml_str, sca_ml_str), &SCA_PATH(sca_ml_str, sca_backslash), &SCA_PATH(sca_ml_str, sca_ml_slash));
	sca_assign_children(&sca_ml_slash, 1, &SCA_PATH(sca_ml_slash, sca_ml_comment));
	sca_assign_children(&sca_ml_comment, 2, &SCA_PATH(sca_ml_comment, sca_ml_comment), &SCA_PATH(sca_ml_comment, sca_ml_str));

	sca_assign_children(&sca_slash, 1, &SCA_PATH(sca_slash, sca_comment));
	sca_assign_children(&sca_comment, 1, &SCA_PATH(sca_comment, sca_comment));
	sca_assign_children(&sca_exclamation, 1, &SCA_PATH(sca_exclamation, sca_notequal));
	sca_assign_children(&sca_greater, 1, &SCA_PATH(sca_greater, sca_greaterequal));
	sca_assign_children(&sca_assign, 1, &SCA_PATH(sca_assign, sca_equal));
	sca_assign_children(&sca_less, 1, &SCA_PATH(sca_less, sca_lessequal));
	// sca_assign_children(&sca_hashtag);
	// orelse unary operator
	sca_assign_children(&sca_dt, 2, &SCA_PATH(sca_dt, sca_orelse_op), &SCA_PATH(sca_dt, sca_dt));

	return scanner;
}

/**
 * @brief Uvolní paměť, která byla alokována pro scanner.
 *
 * Tato funkce uvolní paměť, která byla alokována pro scanner a jeho vnitřní
 * proměnné. Protože scanner používá dynamicky alokovanou paměť, je nutné
 * tuto funkci volat před koncem programu, aby se zabránilo úniku paměti.
 *
 * @param scanner Ukazatel na scanner, jehož paměť má být uvolněna.
 */
void scn_free(Scanner_ptr scanner)
{
	sca_free(&sca_init);
	sca_free(&sca_underscore);
	sca_free(&sca_lexeme);
	sca_free(&sca_opt_str_lexeme);

	sca_free(&sca_qm);
	sca_free(&sca_bro);
	sca_free(&sca_brc);

	sca_free(&sca_at);
	sca_free(&sca_atid);

	sca_free(&sca_dt);
	sca_free(&sca_int);
	sca_free(&sca_intdt);
	sca_free(&sca_intexp);
	sca_free(&sca_intexpsign);
	sca_free(&sca_dec);
	sca_free(&sca_dec2);

	sca_free(&sca_s1);
	sca_free(&sca_s2);
	sca_free(&sca_s3);
	sca_free(&sca_s_max1);
	sca_free(&sca_s_max2);
	sca_free(&sca_s5);

	sca_free(&sca_backslash);
	sca_free(&sca_ml1);
	sca_free(&sca_ml2);
	sca_free(&sca_ml3);
	sca_free(&sca_ml_max1);
	sca_free(&sca_ml_max2);
	sca_free(&sca_ml5);
	sca_free(&sca_ml_str);
	sca_free(&sca_ml_slash);
	sca_free(&sca_ml_comment);

	sca_free(&sca_slash);
	sca_free(&sca_comment);
	sca_free(&sca_exclamation);
	sca_free(&sca_greater);
	sca_free(&sca_assign);
	sca_free(&sca_less);
	sca_free(&sca_dt);

	tok_dll_dispose(scanner->list);
	ifree(scanner->file_name);
	ifree(scanner->source);
	ifree(scanner);
}
/**
 * @brief Vyhleda v poli podminek, zdali je splnena nejaka podminka.
 * @details Vyhleda v poli podminek, zdali je splnena nejaka podminka.
 * Podminky jsou v OR vztahu, takze jakmile najde splnenou podminku, funkce
 * skonci a vraci true. Pokud nesplni zadnou podminku, vraci false.
 * @param path Ukazatel na strukturu Scan_path, ktera obsahuje pole podminek.
 * @param c Char, ktery se ma zkontrolovat.
 * @returns true, pokud byla splnena nejaka podminka, jinak false.
 */
bool sca_p_has_match(Scan_path *path, char c)
{
	for (size_t index = 0; index < path->count; index++)
		if ((bool)(path->matches[index])((int)c))
			return true; // no need to evaluate further, conditions are in an OR relation
	return false;
}

/**
 * @brief Najde cestu z uzlu, ktera vyhovuje danemu znaku.
 * @details Iterativne projde cesty a hleda takovou, ktera vyhovuje danemu znaku.
 * Pokud takova cesta je nalezena, vraci ji. Pokud ne, vraci NULL.
 * @param node Uzel, ktery se ma prohledat.
 * @param c Znak, ktery se ma hledat.
 * @returns Cesta, ktera vyhovuje danemu znaku, nebo NULL, pokud takova cesta neexistuje.
 */
Scan_path *sca_n_has_match(Scan_node *node, char c)
{
	for (int index = 0; index < node->count; index++)
		if (sca_p_has_match(node->children[index], c))
		{
			return node->children[index];
		}
	return NULL;
}
char *scn_parse_multiline(Scanner_ptr scanner, size_t index)
{
	// assert(index >= scanner->source_index); // TODO: replace with something a bit safer, this is A MACRO

	char *result = malloc(sizeof(char) * (index - scanner->source_index + 1));
	if (result == NULL)
	{
		exit_internal();
	}
	memset(result, 0, index - scanner->source_index + 1);

	for (size_t i = scanner->source_index; i < index;)
	{
		char *startline = strchr(scanner->source + i, '\\'); // first ocurence of backslash
		if (startline != NULL && startline[1] == '\\')
		{
			char *endline = strchr(startline + 1, '\n');
			if (endline == NULL)
			{ // means there is no ENDLINE => error
				free(result);
				exit_internal();
			}
			size_t len = endline - startline;
			memcpy(result + strlen(result), startline + 2 + (startline[2] == ' '), len - 2 - (startline[2] == ' '));
			result[strlen(result)] = '\n';
			i += endline - (scanner->source + i) + 1;
		}
		else
		{
			break;
		}
	}
	size_t len = strlen(result);
	char *tmp = realloc(result, len);
	if (NULL == tmp)
	{
		free(result);
		exit_internal();
	}
	else
	{
		tmp[len-1] = '\0';
		result = tmp;
	}
	if (safe_memory)
		memory_ht_insert(&_memory_table, result);
	return result;
}

/**
 * @brief Vrati token, ktery je na danem indexu v zdrojovem textu.
 * @details Funkce projde grafem skeneru a nalezne token, ktery je
 * na danem indexu v zdrojovem textu. Pokud je nalezen token, vraci
 * se ukazatel na strukturu, ktera reprezentuje tento token.
 * Pokud token neexistuje, vraci se NULL.
 * @param scanner Struktura, ktera reprezentuje scanner.
 * @returns Ukazatel na strukturu, ktera reprezentuje token, nebo NULL, pokud token neexistuje.
 */
Token_ptr scn_scan(Scanner_ptr scanner)
{
	size_t high = scanner->source_index;
	Scan_node *node = &sca_init;
	bool has_match = true;
	if (!scanner->is_scanned)
	{
		tok_dll_first(scanner->list);
		while (has_match)
		{
			Scan_path *path = high < scanner->source_size ? sca_n_has_match(node, scanner->source[high]) : NULL;
			if (path != NULL)
			{
				node = path->to;
				high++;
				if (path->to == path->from && path->to == &sca_init)
				{
					scanner->source_index = high;
				}
				if ((scanner->source[high] == '\n'))
				{
					scanner->line++;
					scanner->source_line = high + 1;
				}
			}
			else
			{
				if (node->state == sca_s_comment)
				{
					node = &sca_init;
					scanner->source_index = high;
					continue;
				}
				int offset = (node->state == sca_s_str) * 1;
				char *token_text;
				token_type type = tok_t_eof;
				if (scanner->source_size != scanner->source_index)
				{
					if (node->state != sca_s_ml_str)
					{
						token_text = (char *)imalloc(sizeof(char) * (high - scanner->source_index + 1 - (offset != 0) * 2));
						memcpy(token_text, scanner->source + scanner->source_index + offset, high - scanner->source_index - (offset != 0) * 2);
						token_text[high - scanner->source_index - (offset != 0) * 2] = '\0';
					}
					else
					{
						token_text = scn_parse_multiline(scanner, high);
					}
					type = scn_get_tok_type(node->state, token_text);
				}
				Token_ptr token = tok_init(type);
				if (type == tok_t_str || type == tok_t_mstr || type == tok_t_int || type == tok_t_flt || type == tok_t_sym || type == tok_t_error)
					tok_set_attribute(token, token_text);
				scanner->source_index = high;
				has_match = false;
				tok_dll_push_back(scanner->list, *token);
				if (type != tok_t_eof)
				{
					ifree(token_text);
				}
				else
				{
					scanner->is_scanned = true;
				}
				if (type == tok_t_error)
				{
					char *message = scn_compose_message(scanner);
					exit_lexic(message);
				}
				return token;
			}
		}
	}
	else
	{
		return scn_next(scanner);
	}
	return NULL;
}

/**
 * @brief Ziskani dalsiho tokenu
 *
 * Funkce vraci dalsi token v seznamu a posune aktivni prvek na dalsi.
 *
 * @param scanner Ukazatel na strukturu reprezentujici scanner
 * @return Ukazatel na strukturu reprezentujici token
 */
Token_ptr scn_next(Scanner_ptr scanner)
{
	Token_ptr token = scanner->list->activeElement->ptr;
	tok_dll_next(scanner->list);
	if (token->type == tok_t_eof || token->type == tok_t_error)
		tok_dll_first(scanner->list);
	return token;
}

/**
 * @brief Otevreni souboru se zdrojovym kodem
 *
 * Funkce otevira soubor se zdrojovym kodem, nacte jeho obsah do pameti
 * a vrati ukazatel na dynamicke pole, kde je ulozen zdrojovy kod.
 *
 * @return Ukazatel na dynamicke pole, kde je ulozen zdrojovy kod
 */
char *scn_open_file(Scanner_ptr scanner)
{
	FILE *program = fopen(scanner->file_name, "r");
	if (program == NULL)
	{
		// TODO: throw error err_internal
		exit_internal();
	}

	fseek(program, 0, SEEK_END);
	size_t file_size = ftell(program) + 2;
	fseek(program, 0, SEEK_SET);

	char *source = imalloc(file_size);
	fread(source, 1, file_size, program);
	fclose(program);
	// no newline on the end of the file
	source[file_size - 1] = '\0';
	if (source[file_size - 3] != '\n')
	{
		source[file_size - 2] = '\n';
	}
	return source;
}

/**
 * @brief Kompozice chyboveho hlaseni.
 *
 * Funkce vraci retezec, ktery popisuje lexikalni chybu.
 * Retezec ma format "Lexical error on line <cislo_radky>: <radek_zdrojoveho_kоdу>\n<^>".
 *
 * @param scanner Struktura, ktera reprezentuje scanner.
 * @returns string, ktery popisuje lexikalni chybu.
 */
char *scn_compose_message(Scanner_ptr scanner)
{
	char *preamble = "Lexical error on line";
	// gathering the whole line
	char *end = strchr(&scanner->source[scanner->source_line], '\n');
	if (end != NULL)
		*end = scanner->source[scanner->source_size - 2];
	int len = end - &scanner->source[scanner->source_line];
	if (len < 0)
		exit_internal();
	char *line = imalloc((len) + 1);
	memcpy(line, &scanner->source[scanner->source_line], len);
	// gathering the line number
	size_t number_size = (size_t)(log(scanner->line) / log(10)) + 1;
	// return message composition
	char *message = imalloc((strlen(preamble) + number_size + 1 + len) * 2 + 1);
	size_t nl = sprintf(message, "%s %zu: %s\n", preamble, scanner->line, line);
	ifree(line);
	memset(message + nl, ' ', (strlen(preamble) + number_size + 1 + len));
	message[strlen(preamble) + number_size + len + nl] = '^';
	message[strlen(preamble) + number_size + len + nl + 1] = '\0';
	return message;
}