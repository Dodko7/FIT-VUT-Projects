#include <stdio.h>

#include "../../src/utils/dll.c"
#include "../../src/utils/dll.h"

_DLL(test, t, int)
DLL(test, t, int, nothing, nothing, nothing)


int main() {
	t_dllist *dll = t_dll_init();
	
	t_dll_push_back(dll, 1);
	t_dll_push_back(dll, 2);
	t_dll_push_back(dll, 3);
	t_dll_push_back(dll, 4);
	t_dll_push_back(dll, 5);
	t_dll_push_back(dll, 6);
	t_dll_push_back(dll, 7);
	t_dll_push_back(dll, 8);
	t_dll_push_back(dll, 9);
	t_dll_push_back(dll, 10);
	t_dll_insert(dll, 5, 42);
	for (int i = 0; i < dll->currentLength; ++i) {
		printf("%d ", *t_dll_at(dll, i)->ptr);
	}
	printf("\n");
	t_dll_pop_front(dll);
	t_dll_pop_back(dll);
	t_dll_delete(dll, 3);
	for (int i = 0; i < dll->currentLength; ++i) {
		printf("%d ", *t_dll_at(dll, i)->ptr);
	}
	printf("\n");
	t_dll_clear(dll);
	t_dll_dispose(dll);
	return 0;
}
