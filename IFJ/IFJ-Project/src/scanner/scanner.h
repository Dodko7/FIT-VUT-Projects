/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

/**
 * @brief Scanner
 * JUST FOR CLARITY
 * funkce scn jsou pro externi uziti a primo pracuji se skenerem
 * funkce a makra s prefixem sca jsou POUZE pro interni uziti, s pomoci
 * nichz se primo ovlivnuje graf skeneru, cesty v nem a podminky.
 */
#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include "../utils/token.h"
#include "../utils/token_dll.h"
#include "../utils/token_types.h"
#include "scan_state.h"

#define SCA_MATCH_DECL(name, a) \
	int m_chk_##name(int b) { return b == (a); }
#define SCA_MATCH(name) m_chk_##name

#define SCA_GREATER_DECL(name, a) \
	int m_chk_##name(int b) { return b > (a); }
#define SCA_GREATER(name) m_chk_##name

#define SCA_PATH_DECL(src, dest) Scan_path src##_to_##dest;
#define SCA_PATH_DEF(src, dest, args...) Scan_path src##_to_##dest = {.from = &src, .to = &dest, .matches = NULL, .count = 0};
#define SCA_PATH_INIT(name, args...)                                              \
	name.matches = imalloc(sizeof((int (*[])(int)){args}));                         \
	memcpy(name.matches, (int (*[])(int)){args}, sizeof((int (*[])(int)){args})); \
	name.count = sizeof((int (*[])(int)){args}) / sizeof(int (*)(int));
#define SCA_PATH_DEINIT(name) \
	ifree(name.matches);       \
	name.matches = NULL;
#define SCA_PATH(src, dest) src##_to_##dest

/**
 * @struct _Scanner
 * @brief Struktura reprezentujici scanner
 *
 * Struktura reprezentuje scanner, ktery se pouziva pro lexikalni
 * analyzu. Struktura obsahuje ukazatel na jmeno souboru, ktery
 * se ma analyzovat a ukazatel na dynamicke pole, kam se budou
 * ukladat tokeny, dále aktuální řádku a index v zdrojovém textu.
 */
typedef struct _Scanner
{
	char *file_name;
	tok_dllist *list;
	char *source;
	size_t source_index;
	size_t source_line;
	size_t source_size;
	size_t line;
	bool is_scanned;
} Scanner;

typedef struct _Scanner *Scanner_ptr;
typedef struct Scan_node Scan_node;
typedef struct Scan_path Scan_path;

/**
 * @struct Scan_node
 * @brief Uzel grafu skeneru
 *
 * Struktura reprezentuje jeden uzel v grafu skeneru. Uzel
 * obsahuje pole ukazatelu na dalsi uzly, ktere jsou
 * dostupne z tohoto uzlu.
 */
struct Scan_node
{
	// conditions that are checked in order to determine given character matches the node
	Scan_path **children;
	size_t count;
	scn_state_t state;
};

struct Scan_path
{
	int (**matches)(int);
	size_t count;
	Scan_node *from;
	Scan_node *to;
};

Scanner_ptr scn_init(char *filename);
void scn_free(Scanner_ptr scanner);
Token_ptr scn_scan(Scanner_ptr scanner);

Token_ptr scn_get_token(Scanner_ptr scanner);
Token_ptr scn_previous(Scanner_ptr scanner);
Token_ptr scn_next(Scanner_ptr scanner);
char *scn_open_file(Scanner_ptr scanner);
char *scn_compose_message(Scanner_ptr scanner);
// scanner analyze functions, usage restricted to scanner.c
/// entry function
// Token_ptr sca_init();

typedef Scan_node *Scan_node_ptr;
// extern Scan_path sca_paths[];

void sca_assign_children(Scan_node_ptr node, int argc, ...);
void sca_free(Scan_node_ptr node);

bool sca_p_has_match(Scan_path *path, char c);
Scan_path *sca_n_has_match(Scan_node *node, char c);

#endif