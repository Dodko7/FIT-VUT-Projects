/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Vypíše informaci o souboru a řádku, kde byla nalezena chyba a ukončí program.
 *
 * Toto makro se používá pro ukončení programu v případě, že dojde k vnitřní chybě
 * (tj. chybě, která nebyla způsobena špatným vstupem, ale vnitřní chybou programu).
 */
#define exit_internal() {fprintf(stderr, "%s: %d\n", __FILE__, __LINE__); _exit_internal();}

#define ERR_COUNT 11

typedef enum _err_codes{
	err_none,			// 0 - normální stav
	err_lexic,			// 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému). 
	err_syntax,	   		// 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.). 
	err_undef,			// 3 - sémantická chyba v programu - nedefinovaná funkce či proměnná. 
	err_param,			// 4 - sémantická chyba v programu - špatný počet/typ parametrů u volání funkce; špatný typ či nepovolené zahození návratové hodnoty z funkce. 
	err_redef,			// 5 - sémantická chyba v programu - redefinice proměnné nebo funkce; přiřazení do nemodifikovatelné proměnné. 
	err_ret_val,		// 6 - sémantická chyba v programu - chybějící/přebývající výraz v příkazu návratu z funkce. 
	err_dt_invalid,		// 7 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech; nekompatibilní typ výrazu (např. při přiřazení). 
	err_dt_unknown,		// 8 - sémantická chyba odvození typu - typ proměnné není uveden a nelze odvodit od použitého výrazu. 
	err_dt_unused,		// 9 - sémantická chyba nevyužité proměnné v jejím rozsahu platnosti; modifikovatelná proměnná bez možnosti změny po její inicializaci. 
	err_semantic,		// 10 - ostatní sémantické chyby. 
	err_internal=99 	// 99 - interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.). 
} err_codes;

typedef struct Err_message {
	err_codes code;
	char *format;
} Err_message;

extern const Err_message err_msg[];

size_t err_fetch(err_codes err);
void err_print(err_codes err, bool message_needed);

void exit_lexic(char * what);
void exit_syntax();
void exit_undef();
void exit_param();
void exit_redef();
void exit_ret_val();
void exit_dt_invalid();
void exit_dt_unknown();
void exit_dt_unused();
void exit_semantic();
void _exit_internal();
#endif