#include "hashtable.h"
#include "sll.h"
#include <stdlib.h>
#include <string.h>

#define HT_INIT(entries, prefix, element, key_type)      \
	void prefix##_ht_init(prefix##_ht *table)            \
	{                                                    \
		prefix##_sllist *template = prefix##_sll_init(); \
		for (int i = 0; i < entries; i++)                \
		{                                                \
			((table->table)[i]) = *template;             \
		}                                                \
		free(template);                                  \
	}

/**
 * @brief Search for an element in the hash table.
 *
 * @param table The hash table.
 * @param key The key of the element to search for.
 * @return A pointer to the element if found, NULL otherwise.
 */
#define HT_SEARCH(entries, prefix, element, key_type, hash_fn, comp_fn)           \
	prefix##_sll_element_ptr prefix##_ht_search(prefix##_ht *table, key_type key) \
	{                                                                             \
		size_t hash = hash_fn(key);                                               \
		prefix##_sllist *row = &(table->table)[hash];                             \
		prefix##_sll_first(row);                                                  \
		prefix##_sll_element_ptr item = prefix##_sll_front(row);                  \
		while (prefix##_sll_is_active(row))                                       \
		{                                                                         \
			if (comp_fn == nothing_size)                                          \
			{                                                                     \
				if (*item->ptr == key)                                            \
					return item;                                                  \
			}                                                                     \
			else                                                                  \
			{                                                                     \
				if (comp_fn(*item->ptr, key))                                     \
				{                                                                 \
					return item;                                                  \
				}                                                                 \
			}                                                                     \
			item = prefix##_sll_next(row);                                        \
		}                                                                         \
		return NULL;                                                              \
	}

/**
 * @brief Vlozeni prvku do hashtabulky.
 *
 * @param table Hasovaci tabulka, do ktereho se ma prvek vlozit.
 * @param data Prvek, ktery se ma vlozit.
 */
#define HT_INSERT(entries, prefix, element, key_type, hash_fn, key_fn, copy_fn, deinit_fn) \
	void prefix##_ht_insert(prefix##_ht *table, element data)                              \
	{                                                                                      \
		prefix##_sll_element_ptr item = prefix##_ht_search(table, key_fn(data));           \
		if (item == NULL)                                                                  \
		{                                                                                  \
			size_t hash = hash_fn(key_fn(data));                                           \
			prefix##_sll_push_front(&(table->table)[hash], data);                          \
			table->size++;                                                                 \
		}                                                                                  \
		else                                                                               \
		{                                                                                  \
			deinit_fn(item->ptr);                                                          \
			item->ptr = copy_fn(data);                                                     \
		}                                                                                  \
	}

/**
 * @brief Hledani prvku v hasovaci tabulce.
 *
 * @param table Hasovaci tabulka, ve ktere se ma prvek hledat.
 * @param key Klic, podle ktereho se hleda.
 *
 * @return Ukazatel na nalezeny prvek, nebo NULL, pokud nebyl prvek nalezen.
 */
#define HT_GET(entries, prefix, element, key_type)                      \
	element *prefix##_ht_get(prefix##_ht *table, key_type key)          \
	{                                                                   \
		prefix##_sll_element_ptr item = prefix##_ht_search(table, key); \
		if (item != NULL)                                               \
		{                                                               \
			return item->ptr;                                           \
		}                                                               \
		return NULL;                                                    \
	}

/**
 * @brief Smaze prvek z hasovaci tabulky.
 *
 * @param table Hasovaci tabulka, ze ktere se ma prvek smazat.
 * @param key Klic, podle ktereho se hleda.
 *
 * Pokud je prvek nalezen, funkce ho smaze a dekrementuje velikost tabulky.
 */
#define HT_DELETE(entries, prefix, element, key_type, comp_fn, hash_fn, deinit_fn) \
	void prefix##_ht_delete(prefix##_ht *table, key_type key)                      \
	{                                                                              \
		size_t hash = hash_fn(key);                                                \
		prefix##_sllist *row = &((table->table)[hash]);                            \
		prefix##_sll_first(row);                                                   \
		size_t index = 0;                                                          \
		prefix##_sll_element_ptr item = prefix##_sll_front(row);                   \
		while (prefix##_sll_is_active(row))                                        \
		{                                                                          \
			if (comp_fn == nothing_size)                                           \
			{                                                                      \
				if (*item->ptr == key)                                             \
				{                                                                  \
					prefix##_sll_delete(row, index);                               \
					table->size--;                                                 \
					return;                                                        \
				}                                                                  \
			}                                                                      \
			else                                                                   \
			{                                                                      \
				if (comp_fn(*item->ptr, key))                                      \
				{                                                                  \
					prefix##_sll_delete(row, index);                               \
					table->size--;                                                 \
					return;                                                        \
				}                                                                  \
			}                                                                      \
			item = prefix##_sll_next(row);                                         \
			index++;                                                               \
		}                                                                          \
	}
/**
 * @brief Uvolni pamet alokovanou pro hasovaci tabulku a vsechny jeji prvky.
 *
 * @param table Hasovaci tabulka, ktera se ma uvolnit.
 */
#define HT_DISPOSE(entries, prefix, element, key_type, deinit_fn) \
	void prefix##_ht_dispose(prefix##_ht *table)                  \
	{                                                             \
		for (size_t i = 0; i < entries; i++)                      \
		{                                                         \
			prefix##_sll_dispose(&(table->table)[i]);             \
		}                                                         \
	}

/**
 * @def HASHTABLE(entries, prefix, element, key_type, hash_fn, key_fn, copy_fn, comp_fn, deinit_fn)
 * @brief Makro pro definici funkci hasovaci tabulky.
 *
 * @param entries pocet polozek v tabulce
 * @param prefix prefix jmena struktur a funkci
 * @param element typ prvku ulozenych v tabulce
 * @param key_type typ klice prvku
 * @param hash_fn funkce pro vypocet hashe z klice
 * @param key_fn funkce pro ziskani klice z prvku
 * @param copy_fn funkce pro kopirovani prvku
 * @param comp_fn funkce pro porovnani dvou klicu
 * @param deinit_fn funkce pro uvolneni pameti prvku
 */
#define HASHTABLE(entries, prefix, element, key_type, hash_fn, key_fn, copy_fn, comp_fn, deinit_fn) \
	HT_INIT(entries, prefix, element, key_type)                                                     \
	HT_SEARCH(entries, prefix, element, key_type, hash_fn, comp_fn)                                 \
	HT_INSERT(entries, prefix, element, key_type, hash_fn, key_fn, copy_fn, deinit_fn)              \
	HT_GET(entries, prefix, element, key_type)                                                      \
	HT_DELETE(entries, prefix, element, key_type, comp_fn, hash_fn, deinit_fn)                      \
	HT_DISPOSE(entries, prefix, element, key_type, deinit_fn)
