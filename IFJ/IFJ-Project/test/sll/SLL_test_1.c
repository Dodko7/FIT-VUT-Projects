#include <stdio.h>

#include "../../src/utils/sll.c"
#include "../../src/utils/sll.h"

/**
 * @file DLL_test_1.c
 * @brief Testovaci soubor pro dynamicky alokovany spojovy seznam
 *
 * Modul obsahuje test inicializace pro dynamicky alokovany spojovy seznam.
 * seznam je iniciovan, je do nej vlozeno, a pote je z nej odstraneno.
 * testovane funkce
 * - inicializace
 * - vlozeni dopredu
 * - vlozeni na index
 * - vymazani prvku
 * - deinicializace
 *
 * @author xramas01 Jakub Ramaseuski
 */

_SLL(test, t, int)
SLL(test, t, int, nothing, nothing, true, false)
void sll_print(t_sllist *sll) {
	t_sll_element_ptr temp = sll->firstElement;
	while (temp != NULL) {
		printf("%d ", *temp->ptr);
		temp = temp->next;
	}
	printf("\n");
}

int main() {
	t_sllist *sll = t_sll_init();
	
	t_sll_push_front(sll, 1);
	t_sll_push_front(sll, 2);
	t_sll_push_front(sll, 3);
	t_sll_push_front(sll, 4);
	t_sll_push_front(sll, 5);
	t_sll_push_front(sll, 6);
	t_sll_push_front(sll, 7);
	t_sll_push_front(sll, 8);
	t_sll_push_front(sll, 9);
	t_sll_push_front(sll, 10);
	t_sll_push_front(sll, 11);
	t_sll_insert(sll, 5, 42);
	sll_print(sll);
	t_sll_clear(sll);
	sll_print(sll);
	t_sll_dispose(sll);
	return 0;
}