#include <stdio.h>

#define DLL_TRAD
#include "../../src/utils/dll.c"
#include "../../src/utils/dll.h"
#include "../../src/utils/token.h"

/**
 * @file DLL_trad_test_0.c
 * @brief Testovaci soubor pro tradicni dynamicky alokovany spojovy seznam
 * (Token)
 * Modul obsahuje test inicializace pro tradicni dynamicky alokovany spojovy seznam s datovym typem Token.
 *
 * @author xramas01 Jakub Ramaseuski
 */

_DLL(test, tok, Token)
DLL(test, tok, Token, tok_copy, tok_init_no_ret, tok_free)
void dll_print(tok_dllist *dll)
{
	tok_dll_element_ptr temp = dll->firstElement;
	while (temp != NULL)
	{
		printf("%s ", kw_get_lexeme(temp->ptr->type));
		temp = temp->next;
	}
	printf("\n");
}

int main()
{
	tok_dllist *dll = malloc(sizeof(*dll)); 
	tok_dll_init(dll);

	Token_ptr constTok = tok_init(tok_t_const);
	Token_ptr elseTok = tok_init(tok_t_else);
	Token_ptr fnTok = tok_init(tok_t_fn);
	Token_ptr ifTok = tok_init(tok_t_if);
	Token_ptr i32Tok = tok_init(tok_t_i32);
	Token_ptr f64Tok = tok_init(tok_t_f64);
	Token_ptr nullTok = tok_init(tok_t_null);
	Token_ptr pubTok = tok_init(tok_t_pub);
	Token_ptr returnTok = tok_init(tok_t_return);
	Token_ptr u8Tok = tok_init(tok_t_u8);
	Token_ptr voidTok = tok_init(tok_t_void);
	Token_ptr whileTok = tok_init(tok_t_while);
	tok_dll_push_front(dll, *constTok);
	tok_dll_push_back(dll, *elseTok);
	tok_dll_push_back(dll, *fnTok);
	tok_dll_push_back(dll, *ifTok);
	tok_dll_push_back(dll, *i32Tok);
	tok_dll_push_back(dll, *f64Tok);
	tok_dll_push_back(dll, *nullTok);
	tok_dll_push_back(dll, *pubTok);
	tok_dll_push_back(dll, *returnTok);
	tok_dll_push_back(dll, *u8Tok);
	tok_dll_push_back(dll, *voidTok);
	tok_dll_push_back(dll, *whileTok);
	tok_dll_insert(dll, 5, *whileTok);
	dll_print(dll);
	tok_dll_clear(dll);
	dll_print(dll);
	tok_dll_dispose(dll);
	free(constTok);
	free(elseTok);
	free(fnTok);
	free(ifTok);
	free(i32Tok);
	free(f64Tok);
	free(nullTok);
	free(pubTok);
	free(returnTok);
	free(u8Tok);
	free(voidTok);
	free(whileTok);
	return 0;
}