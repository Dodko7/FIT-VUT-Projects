#include <stdio.h>

#include "../../src/utils/dll.c"
#include "../../src/utils/dll.h"

/**
 * @file DLL_test_3.c
 * @brief Testovaci soubor pro dynamicky alokovany spojovy seznam
 * (char*)
 * Modul obsahuje test inicializace pro dynamicky alokovany spojovy seznam s datovym typem char*.
 *
 * @author xramas01 Jakub Ramaseuski
 */

_DLL(test, t, char *)
DLL(test, t, char *, nothing, nothing, nothing)
void dll_print(t_dllist *dll) {
	t_dll_element_ptr temp = dll->firstElement;
	while (temp != NULL) {
		printf("%s ", *temp->ptr);
		temp = temp->next;
	}
	printf("\n");
}

int main() {
	t_dllist *dll = t_dll_init();
	
	t_dll_push_front(dll, "Lorem");
	t_dll_push_back(dll, "ipsum");
	t_dll_push_back(dll, "dolor");
	t_dll_push_back(dll, "sit");
	t_dll_push_back(dll, "amet");
	t_dll_push_back(dll, "consectetur");
	t_dll_push_back(dll, "adipiscing");
	t_dll_push_back(dll, "elit");
	t_dll_push_back(dll, "sed");
	t_dll_push_back(dll, "do");
	t_dll_push_back(dll, "eiusmod");
	t_dll_push_back(dll, "tempor");
	t_dll_insert(dll, 5, "incididunt");
	dll_print(dll);
	t_dll_clear(dll);
	dll_print(dll);
	t_dll_dispose(dll);
	return 0;
}