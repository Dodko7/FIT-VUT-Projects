#ifndef DLL_H
#define DLL_H
#include <stdbool.h>

#define _DLL_ELEM(name, prefix, element)                              \
	typedef struct _##prefix##_dll_element                            \
	{                                                                 \
		element *ptr;                                                 \
		struct _##prefix##_dll_element *next;                         \
		struct _##prefix##_dll_element *previous;                     \
	} prefix##_dll_element;                                           \
                                                                      \
	typedef struct _##prefix##_dll_element *prefix##_dll_element_ptr; \
	typedef struct                                                    \
	{                                                                 \
		prefix##_dll_element_ptr firstElement;                        \
		prefix##_dll_element_ptr activeElement;                       \
		prefix##_dll_element_ptr lastElement;                         \
		size_t currentLength;                                         \
	} prefix##_dllist;

#ifdef DLL_TRAD
#define _DLL_FUNCTIONS(name, prefix, element)                                             \
	void prefix##_dll_init(prefix##_dllist *dll);                                         \
	bool prefix##_dll_clear(prefix##_dllist *dll);                                        \
	bool prefix##_dll_dispose(prefix##_dllist *dll);                                      \
	bool prefix##_dll_insert(prefix##_dllist *dll, size_t index, element data);           \
	bool prefix##_dll_delete(prefix##_dllist *dll, size_t index);                         \
	bool prefix##_dll_push_back(prefix##_dllist *dll, element data);                      \
	bool prefix##_dll_push_front(prefix##_dllist *dll, element data);                     \
	bool prefix##_dll_pop_back(prefix##_dllist *dll);                                     \
	bool prefix##_dll_pop_front(prefix##_dllist *dll);                                    \
	void prefix##_dll_first(prefix##_dllist *dll);                                        \
	void prefix##_dll_last(prefix##_dllist *dll);                                         \
	bool prefix##_dll_is_active(prefix##_dllist *dll);                                    \
	prefix##_dll_element_ptr prefix##_dll_front(prefix##_dllist *dll);                    \
	prefix##_dll_element_ptr prefix##_dll_back(prefix##_dllist *dll);                     \
	prefix##_dll_element_ptr prefix##_dll_at(prefix##_dllist *dll, size_t index);         \
	prefix##_dll_element_ptr prefix##_dll_set(prefix##_dllist *dll, size_t index);        \
	prefix##_dll_element_ptr prefix##_dll_set_active(prefix##_dllist *dll, size_t index); \
	prefix##_dll_element_ptr prefix##_dll_set_first(prefix##_dllist *dll, size_t index);  \
	prefix##_dll_element_ptr prefix##_dll_set_last(prefix##_dllist *dll, size_t index);   \
	void prefix##_dll_next(prefix##_dllist *dll);                                         \
	void prefix##_dll_previous(prefix##_dllist *dll);
#else
#define _DLL_FUNCTIONS(name, prefix, element)                                     \
	prefix##_dllist *prefix##_dll_init();                                         \
	void prefix##_dll_clear(prefix##_dllist *dll);                                \
	void prefix##_dll_dispose(prefix##_dllist *dll);                              \
	void prefix##_dll_insert(prefix##_dllist *dll, size_t index, element data);   \
	void prefix##_dll_delete(prefix##_dllist *dll, size_t index);                 \
	void prefix##_dll_push_back(prefix##_dllist *dll, element data);              \
	void prefix##_dll_push_front(prefix##_dllist *dll, element data);             \
	void prefix##_dll_pop_back(prefix##_dllist *dll);                             \
	void prefix##_dll_pop_front(prefix##_dllist *dll);                            \
	void prefix##_dll_first(prefix##_dllist *dll);                                \
	void prefix##_dll_last(prefix##_dllist *dll);                                 \
	bool prefix##_dll_is_active(prefix##_dllist *dll);                            \
	prefix##_dll_element_ptr prefix##_dll_front(prefix##_dllist *dll);            \
	prefix##_dll_element_ptr prefix##_dll_back(prefix##_dllist *dll);             \
	prefix##_dll_element_ptr prefix##_dll_at(prefix##_dllist *dll, size_t index); \
	prefix##_dll_element_ptr prefix##_dll_next(prefix##_dllist *dll);             \
	prefix##_dll_element_ptr prefix##_dll_previous(prefix##_dllist *dll);
#endif

/**
 * @brief Definice makra pro dvousměrně vázaný seznam
 *
 * Toto makro definuje struktury a funkce pro dvousměrně vázaný seznam
 * pro daný typ elementu. Obsahuje definice pro elementy a funkce
 * pro manipulaci se seznamem.
 *
 * @param prefix Prefix pro názvy struktur a funkcí
 * @param element Typ elementu, který bude seznam obsahovat
 */
#define _DLL(name, prefix, element) \
	_DLL_ELEM(name, prefix, element) \
	_DLL_FUNCTIONS(name, prefix, element)

#ifndef VOID_NOTHING
#define VOID_NOTHING
/**
 * @brief Nic nedela
 *
 * Funkce nic nedela, je zde z duvodu, aby v pripade, kdy
 * element nema zadnou specialni deiniciacni funkci mohl byt vlozen do makra.
 */
void nothing()
{

}
#endif
#endif