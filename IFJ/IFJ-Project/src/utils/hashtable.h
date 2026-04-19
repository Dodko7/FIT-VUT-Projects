#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stddef.h>
#include "sll.h"

/**
 * @def _HASHTABLE(entries, prefix, element, key_type)
 * @brief Definice hasovaci tabulky.
 *
 * @param entries pocet polozek v tabulce
 * @param prefix prefix jmena struktur a funkci
 * @param element typ prvku ulozenych v tabulce
 * @param key_type typ klice prvku
 */
#define _HASHTABLE(entries, prefix, element, key_type)                             \
	_SLL("ht", prefix, element)                                                    \
	typedef struct _##prefix##_ht                                                  \
	{                                                                              \
		prefix##_sllist table[entries];                                            \
		size_t size;                                                               \
	} prefix##_ht;                                                                 \
	void prefix##_ht_init(prefix##_ht *table);                                     \
	prefix##_sll_element_ptr prefix##_ht_search(prefix##_ht *table, key_type key); \
	void prefix##_ht_insert(prefix##_ht *table, element data);                     \
	element *prefix##_ht_get(prefix##_ht *table, key_type key);                    \
	void prefix##_ht_delete(prefix##_ht *table, key_type key);                     \
	void prefix##_ht_dispose(prefix##_ht *table);

#ifndef SIZE_NOTHING
#define SIZE_NOTHING
/**
 * @brief Nic nedela
 *
 * Funkce nic nedela, je zde z duvodu, aby v pripade, kdy
 * element nema zadnou konkretni funkci pro dany ucel.
 */
size_t nothing_size()
{
	return 0;
}
#endif
#endif