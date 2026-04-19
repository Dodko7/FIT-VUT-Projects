#include "scan_state.h"
#include "../utils/errors.h"
#include <stddef.h>

State_type_pair sca_translation_table[] =
	{
		{sca_s_underscore, tok_t_unused},
		{sca_s_lexeme, tok_t_sym},
		{sca_s_opt_str_lexeme, tok_t_sym},

		{sca_s_atid, tok_t_sym},

		{sca_s_dt, tok_t_dot},

		{sca_s_int, tok_t_int},
		{sca_s_dec, tok_t_flt},
		{sca_s_dec2, tok_t_flt},

		{sca_s_str, tok_t_str},

		{sca_s_ml_str, tok_t_mstr},

		{sca_s_eof, tok_t_eof},
		{sca_s_curlybrace_close, tok_t_rcbr},
		{sca_s_curlybrace_open, tok_t_lcbr},
		{sca_s_parenthese_open, tok_t_lpa},
		{sca_s_parenthese_close, tok_t_rpa},
		{sca_s_semicolon, tok_t_semicolon},
		{sca_s_slash, tok_t_divide},
		{sca_s_comment, tok_t_doc},
		{sca_s_vertical, tok_t_alias},
		{sca_s_colon, tok_t_colon},
		{sca_s_exclamation, tok_t_not},
		{sca_s_notequal, tok_t_neq},
		{sca_s_greater, tok_t_gt},
		{sca_s_greaterequal, tok_t_geq},
		{sca_s_assign, tok_t_ass},
		{sca_s_equal, tok_t_eq},
		{sca_s_less, tok_t_lt},
		{sca_s_lessequal, tok_t_leq},
		{sca_s_asterisk, tok_t_times},
		{sca_s_minus, tok_t_minus},
		{sca_s_plus, tok_t_plus},
		{sca_s_comma, tok_t_com},
		{sca_s_hashtag, tok_t_hashtag},
		{sca_s_orelse_op, tok_t_orelse_un},
};

/**
 * @brief Prevodni funkce z internalniho stavu scanneru na typ tokenu.
 * @details Funkce bere jako parametr interni stav scanneru a lexem, ktery
 * byl v danem stavu nalezen. Vraci se typ tokenu, ktery odpovida bud stavu, nebo lexemu.
 * Pokud lexem oznaceny za symbol neni v tabulce keywords, vraci se tok_t_sym. Pokud pro dany
 * interni stav ni mozno vratit tok_t_sym, vraci se tok_t_error.
 * @param state internalni stav scanneru
 * @param lexeme lexem, ktery byl v danem stavu nalezen
 * @return token typu, ktery se rovnaji lexemu
 */
token_type scn_get_tok_type(scn_state_t state, char *lexeme)
{
	for (size_t i = 0; i < sizeof(sca_translation_table) / sizeof(State_type_pair); i++)
	{
		if (sca_translation_table[i].state == state)
		{
			if (sca_translation_table[i].tok_type == tok_t_sym)
			{
				token_type type = kw_get_type(lexeme);
				return ((state == sca_s_atid || state == sca_s_opt_str_lexeme) && type == tok_t_sym) ? tok_t_error : type;
			}
			return sca_translation_table[i].tok_type;
		}
	}
	// error-worthy: ?[]i32, ?[ ]u8 and states that do not have a translation
	return tok_t_error;
}