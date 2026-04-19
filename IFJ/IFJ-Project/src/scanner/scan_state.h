#ifndef SCAN_STATE_H
#define SCAN_STATE_H

#include "../utils/token_types.h"

typedef enum
{
	sca_s_init,

	sca_s_underscore,
	sca_s_lexeme,
	sca_s_opt_str_lexeme,

	sca_s_qm,
	sca_s_bro,
	sca_s_brc,

	sca_s_at,
	sca_s_atid,

	sca_s_dt,
	sca_s_orelse_op,
	sca_s_int,
	sca_s_intdt,
	sca_s_intexp,
	sca_s_intexpsign,
	sca_s_dec,
	sca_s_dec2,

	sca_s_s1,
	sca_s_s2,
	sca_s_s3,
	sca_s_s_max1,
	sca_s_s_max2,
	sca_s_s5,
	sca_s_str,

	sca_s_backslash,
	sca_s_ml1,
	sca_s_ml2,
	sca_s_ml3,
	sca_s_ml_max1,
	sca_s_ml_max2,
	sca_s_ml5,
	sca_s_ml6,
	sca_s_ml_str,
	sca_s_ml_slash,
	sca_s_ml_comment,

	sca_s_eof,
	sca_s_curlybrace_close,
	sca_s_curlybrace_open,
	sca_s_parenthese_open,
	sca_s_parenthese_close,
	sca_s_semicolon,
	sca_s_slash,
	sca_s_comment,
	sca_s_vertical,
	sca_s_colon,
	sca_s_exclamation,
	sca_s_notequal,
	sca_s_greater,
	sca_s_greaterequal,
	sca_s_assign,
	sca_s_equal,
	sca_s_less,
	sca_s_lessequal,
	sca_s_asterisk,
	sca_s_minus,
	sca_s_plus,
	sca_s_comma,
	sca_s_hashtag,

} scn_state_t;

typedef struct
{
	scn_state_t state;
	token_type tok_type;
} State_type_pair;

token_type scn_get_tok_type(scn_state_t state, char *lexeme);
token_type scn_get_keyword(scn_state_t state);

extern State_type_pair sca_tr_table[]; // translation table

#endif