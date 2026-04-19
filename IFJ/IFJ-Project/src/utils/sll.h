#ifndef SLL_H
#define SLL_H
#include <stdbool.h>

#define _SLL_ELEM(name, prefix, element)                              \
	typedef struct _##prefix##_sll_element                            \
	{                                                                 \
		element *ptr;                                                 \
		struct _##prefix##_sll_element *next;                         \
	} prefix##_sll_element;                                           \
                                                                      \
	typedef struct _##prefix##_sll_element *prefix##_sll_element_ptr; \
	typedef struct                                                    \
	{                                                                 \
		prefix##_sll_element_ptr firstElement;                        \
		prefix##_sll_element_ptr activeElement;                       \
		size_t currentLength;                                         \
	} prefix##_sllist;


#define _SLL_FUNCTIONS(name, prefix, element)                                     \
	prefix##_sllist *prefix##_sll_init();                                         \
	void prefix##_sll_clear(prefix##_sllist *sll);                                \
	void prefix##_sll_dispose(prefix##_sllist *sll);                              \
	void prefix##_sll_insert(prefix##_sllist *sll, size_t index, element data);   \
	void prefix##_sll_delete(prefix##_sllist *sll, size_t index);                 \
	void prefix##_sll_push_front(prefix##_sllist *sll, element data);             \
	void prefix##_sll_pop_front(prefix##_sllist *sll);                            \
	void prefix##_sll_first(prefix##_sllist *sll);                                \
	bool prefix##_sll_is_active(prefix##_sllist *sll);                            \
	prefix##_sll_element_ptr prefix##_sll_front(prefix##_sllist *sll);            \
	prefix##_sll_element_ptr prefix##_sll_at(prefix##_sllist *sll, size_t index); \
	prefix##_sll_element_ptr prefix##_sll_next(prefix##_sllist *sll);             \
	prefix##_sll_element_ptr prefix##_sll_previous(prefix##_sllist *sll);

#define _SLL(name, prefix, element)  \
	_SLL_ELEM(name, prefix, element) \
	_SLL_FUNCTIONS(name, prefix, element)

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