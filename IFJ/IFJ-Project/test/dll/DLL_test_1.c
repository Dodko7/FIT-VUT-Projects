#include <stdio.h>

#include "../../src/utils/dll.c"
#include "../../src/utils/dll.h"

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

_DLL(test, t, int)
DLL(test, t, int, nothing, nothing, nothing)
void dll_print(t_dllist *dll) {
	t_dll_element_ptr temp = dll->firstElement;
	while (temp != NULL) {
		printf("%d ", *temp->ptr);
		temp = temp->next;
	}
	printf("\n");
}

int main() {
	t_dllist *dll = t_dll_init();
	
	t_dll_push_front(dll, 1);
	t_dll_push_front(dll, 2);
	t_dll_push_front(dll, 3);
	t_dll_push_front(dll, 4);
	t_dll_push_front(dll, 5);
	t_dll_push_front(dll, 6);
	t_dll_push_front(dll, 7);
	t_dll_push_front(dll, 8);
	t_dll_push_front(dll, 9);
	t_dll_push_front(dll, 10);
	t_dll_push_front(dll, 11);
	t_dll_insert(dll, 5, 42);
	dll_print(dll);
	t_dll_clear(dll);
	dll_print(dll);
	t_dll_dispose(dll);
	return 0;
}