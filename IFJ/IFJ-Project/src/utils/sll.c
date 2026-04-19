/**
 * @file sll.c
 * @brief Dynamicky alokovany spojovy seznam
 *
 * Modul obsahuje funkci pro inicializaci seznamu, vkladani a odebrani prvku
 * a funkcionality pro praci s aktivnim prvkem.
 *
 * @author xramas01 Jakub Ramaseuski
 * @cite xsidlil00 Lukas Sidlik
 */
#include "memory_table.h"
#include <stdlib.h>
#include "sll.h"
#ifndef SLL_C
#define SLL_C

/**
 * @brief Inicializace dynamicky alokovaneho seznamu
 *
 * Funkce inicializuje dynamicky alokovany seznam. Vraci ukazatel na
 * strukturu seznamu, nebo NULL, pokud selhalo alokovani pameti.
 *
 * @return Ukazatel na inicializovanou strukturu seznamu, nebo NULL.
 */
#define SLL_INIT(name, prefix, element, deallocate, safe_memory) \
	prefix##_sllist *prefix##_sll_init()                         \
	{                                                            \
		prefix##_sllist *sll;                                    \
		if (safe_memory)                                         \
			sll = imalloc(sizeof(*sll));                         \
		else                                                     \
			sll = malloc(sizeof(*sll));                          \
		if (sll == NULL)                                         \
		{                                                        \
			return NULL;                                         \
		}                                                        \
		sll->firstElement = NULL;                                \
		sll->currentLength = 0;                                  \
		return sll;                                              \
	}

/**
 * @brief Vymaze vsechny prvky seznamu
 *
 * Funkce vymaze vsechny prvky seznamu a uvolni pamet.
 */
#define SLL_CLEAR(name, prefix, element, deallocate, safe_memory) \
	void prefix##_sll_clear(prefix##_sllist *sll)                 \
	{                                                             \
		while (sll->firstElement != NULL)                         \
		{                                                         \
			prefix##_sll_element_ptr temp = sll->firstElement;    \
			sll->firstElement = sll->firstElement->next;          \
			deallocate(temp->ptr);                                \
			if (safe_memory)                                      \
			{                                                     \
				if (deallocate == nothing)                        \
					ifree(temp->ptr);                             \
				ifree(temp);                                      \
			}                                                     \
			else                                                  \
			{                                                     \
				if (deallocate == nothing)                        \
					free(temp->ptr);                              \
				free(temp);                                       \
			}                                                     \
		}                                                         \
	}

/**
 * @brief Uvolni pamet alokovanou pro seznam
 *
 * Funkce uvolni pamet alokovanou pro seznam a vsechny jeho prvky.
 */
#define SLL_DISPOSE(name, prefix, element, deallocate, is_erasable, safe_memory) \
	void prefix##_sll_dispose(prefix##_sllist *sll)                              \
	{                                                                            \
		prefix##_sll_clear(sll);                                                 \
		if (is_erasable)                                                         \
			free(sll);                                                           \
	}

/**
 * @brief Vlozeni prvku do seznamu na pozici index
 *
 * Funkce vlozi prvek s hodnotou data do seznamu na pozici index.
 * Alokujem pamet pro novy prvek, protoze funkci je dano, ze data jsou validni.
 * V pripade, ze selhalo alokovani pameti, funkce nic nedela.
 *
 * @param sll Ukazatel na seznam, do ktereho se ma prvek vlozit.
 * @param index Pozice, na kterou se ma prvek vlozit.
 * @param data Hodnota prvku, ktery se ma vlozit.
 */
#define SLL_INSERT(name, prefix, element, copy, deallocate, safe_memory)       \
	void prefix##_sll_insert(prefix##_sllist *sll, size_t index, element data) \
	{                                                                          \
		prefix##_sll_element_ptr newElement;                                   \
		if (safe_memory)                                                       \
			newElement = imalloc(sizeof(struct _##prefix##_sll_element));      \
		else                                                                   \
			newElement = malloc(sizeof(struct _##prefix##_sll_element));       \
		if (newElement == NULL)                                                \
		{                                                                      \
			return;                                                            \
		}                                                                      \
		if (safe_memory)                                                       \
			newElement->ptr = imalloc(sizeof(element));                        \
		else                                                                   \
			newElement->ptr = malloc(sizeof(element));                         \
		if (newElement->ptr == NULL)                                           \
		{                                                                      \
			return;                                                            \
		}                                                                      \
		if (copy != nothing)                                                   \
			copy(newElement->ptr, data);                                       \
		else                                                                   \
			*newElement->ptr = data;                                           \
		if (index == 0)                                                        \
		{                                                                      \
			newElement->next = sll->firstElement;                              \
			sll->firstElement = newElement;                                    \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			prefix##_sll_element_ptr temp = prefix##_sll_at(sll, index - 1);   \
			if (temp->next != NULL)                                            \
			{                                                                  \
				newElement->next = temp->next;                                 \
			}                                                                  \
			else                                                               \
			{                                                                  \
				newElement->next = NULL;                                       \
			}                                                                  \
			temp->next = newElement;                                           \
		}                                                                      \
		sll->currentLength++;                                                  \
	}

/**
 * @brief Odstrani prvek na pozici index
 *
 * Funkce odstrani prvek na pozici index v seznamu. Pokud je seznam
 * prazdny, nebo je pozice mimo rozsah, funkce nic nedela.
 *
 * @param sll Ukazatel na seznam, ze ktereho se ma prvek odstranit.
 * @param index Pozice, na ktere se ma prvek odstranit.
 */
#define SLL_DELETE(name, prefix, element, deallocate, safe_memory)       \
	void prefix##_sll_delete(prefix##_sllist *sll, size_t index)         \
	{                                                                    \
		if (index == 0)                                                  \
		{                                                                \
			prefix##_sll_element_ptr temp = sll->firstElement;           \
			sll->firstElement = temp->next;                              \
			if (temp != NULL && temp->ptr != NULL)                       \
			{                                                            \
				deallocate(temp->ptr);                                   \
				if (safe_memory)                                         \
				{                                                        \
					if (deallocate == nothing)                           \
						ifree(temp->ptr);                                \
					ifree(temp);                                         \
				}                                                        \
				else                                                     \
				{                                                        \
					if (deallocate == nothing)                           \
						free(temp->ptr);                                 \
					free(temp);                                          \
				}                                                        \
				sll->currentLength--;                                    \
			}                                                            \
			return;                                                      \
		}                                                                \
		prefix##_sll_element_ptr temp = prefix##_sll_at(sll, index - 1); \
		prefix##_sll_element_ptr target = NULL;                          \
		if (temp != NULL && temp->next != NULL)                          \
		{                                                                \
			target = temp->next;                                         \
			temp->next = temp->next->next;                               \
		}                                                                \
		if (target != NULL)                                              \
		{                                                                \
			deallocate(target->ptr);                                     \
			if (safe_memory)                                             \
			{                                                            \
				if (deallocate == nothing)                               \
					free(target->ptr);                                   \
				free(target);                                            \
			}                                                            \
			else                                                         \
			{                                                            \
				if (deallocate == nothing)                               \
					free(target->ptr);                                     \
				free(target);                                              \
			}                                                            \
			sll->currentLength--;                                        \
		}                                                                \
	}

/**
 * @brief Nastavi aktivni prvek na prvni prvek v seznamu
 *
 * Funkce nastavi aktivni prvek na prvni prvek v seznamu.
 *
 * @param sll Ukazatel na seznam, v nemz se nastavuje aktivni prvek.
 */
#define SLL_FIRST(name, prefix, element, deallocate) \
	void prefix##_sll_first(prefix##_sllist *sll)    \
	{                                                \
		sll->activeElement = sll->firstElement;      \
	}

/**
 * @brief Prida prvek na zacatek seznamu
 *
 * Funkce prida prvek s hodnotou data na zacatek seznamu.
 *
 * @param sll Ukazatel na seznam, do ktereho se ma prvek vlozit.
 * @param data Hodnota prvku, ktery se ma vlozit.
 */
#define SLL_PUSH_FRONT(name, prefix, element, deallocate)            \
	void prefix##_sll_push_front(prefix##_sllist *sll, element data) \
	{                                                                \
		prefix##_sll_insert(sll, 0, data);                           \
	}

/**
 * @brief Odstrani prvni prvek z  seznamu
 *
 * Funkce odstrani prvni prvek z  seznamu.
 *
 * @param sll Ukazatel na seznam, z  kter ho se ma prvek odebrat.
 */
#define SLL_POP_FRONT(name, prefix, element, deallocate) \
	void prefix##_sll_pop_front(prefix##_sllist *sll)    \
	{                                                    \
		prefix##_sll_delete(sll, 0);                     \
	}
/**
 * @brief Vrati ukazatel na prvek seznamu na pozici index
 *
 * Funkce vrati ukazatel na prvek seznamu, ktery se nachazi na pozici index.
 *
 * @param sll Ukazatel na seznam, v nemz se hleda prvek.
 * @param index Pozice, na ktere se hleda prvek.
 * @return Ukazatel na prvek nebo NULL, pokud je seznam prazdny nebo je pozice mimo rozsah.
 */
#define SLL_AT(name, prefix, element, deallocate)                                \
	prefix##_sll_element_ptr prefix##_sll_at(prefix##_sllist *sll, size_t index) \
	{                                                                            \
		if (index >= sll->currentLength)                                         \
		{                                                                        \
			return NULL;                                                         \
		}                                                                        \
		prefix##_sll_element_ptr temp = sll->firstElement;                       \
		for (size_t i = 0; i < index; i++)                                       \
		{                                                                        \
			temp = temp->next;                                                   \
		}                                                                        \
		return temp;                                                             \
	}

/**
 * @brief Vrati dalsi prvek v seznamu a posune aktivni prvek na dalsi pozici
 *
 * Funkce vrati dalsi prvek v seznamu, posune aktivni prvek na dalsi pozici a vrati jej.
 * Pokud je seznam prazdny nebo je aktivni prvek posledni v seznamu, vrati NULL.
 *
 * @param sll Ukazatel na seznam, v nemz se hleda dalsi prvek.
 * @return Ukazatel na dalsi prvek nebo NULL, pokud je seznam prazdny nebo je aktivni
 * prvek posledni v seznamu.
 */
#define SLL_NEXT(name, prefix, element, deallocate)                  \
	prefix##_sll_element_ptr prefix##_sll_next(prefix##_sllist *sll) \
	{                                                                \
		if (sll->firstElement == NULL || sll->activeElement == NULL) \
		{                                                            \
			return NULL;                                             \
		}                                                            \
		sll->activeElement = sll->activeElement->next;               \
		return sll->activeElement;                                   \
	}

/**
 * @brief Vrati true, pokud je aktivni prvek nastaven
 *
 * Funkce vrati true, pokud je aktivni prvek nastaven, jinak false.
 *
 * @param sll Ukazatel na seznam, v nemz se testuje aktivni prvek.
 * @return True, pokud je aktivni prvek nastaven, jinak false.
 */
#define SLL_IS_ACTIVE(name, prefix, element, deallocate) \
	bool prefix##_sll_is_active(prefix##_sllist *sll)    \
	{                                                    \
		return sll->activeElement != NULL;               \
	}

/**
 * @brief Vrati prvek na zacatku seznamu
 *
 * Funkce vrati prvek na zacatku seznamu.
 *
 * @param sll Ukazatel na seznam, ze ktereho se ma prvek ziskat.
 * @return Ukazatel na prvek na zacatku seznamu nebo NULL, pokud je seznam prazdny.
 */
#define SLL_FRONT(name, prefix, element, deallocate)                  \
	prefix##_sll_element_ptr prefix##_sll_front(prefix##_sllist *sll) \
	{                                                                 \
		return sll->firstElement;                                     \
	}

/**
 * @def SLL(name, prefix, element, copy, deallocate, is_erasable, safe_memory)
 *
 * @brief Makro pro generovani funkcionalit pro jednosmerne vazany seznam
 *
 * @param name Jmeno seznamu
 * @param prefix Prefix, ktery se ma pouzit u funkci
 * @param element Typ prvku v seznamu
 * @param copy Funkce pro kopirovani prvku
 * @param deallocate Funkce pro uvolneni pameti prvku
 * @param is_erasable Funkce pro test, zda prvky lze uvolnit
 * @param safe_memory Kdyz je true, pouzije se funkce ifree() namisto free()
 */
#define SLL(name, prefix, element, copy, deallocate, is_erasable, safe_memory) \
	SLL_INIT(name, prefix, element, deallocate, safe_memory)                   \
	SLL_INSERT(name, prefix, element, copy, deallocate, safe_memory)           \
	SLL_DELETE(name, prefix, element, deallocate, safe_memory)                 \
	SLL_PUSH_FRONT(name, prefix, element, deallocate)                          \
	SLL_POP_FRONT(name, prefix, element, deallocate)                           \
	SLL_AT(name, prefix, element, deallocate)                                  \
	SLL_NEXT(name, prefix, element, deallocate)                                \
	SLL_DISPOSE(name, prefix, element, deallocate, is_erasable, safe_memory)   \
	SLL_CLEAR(name, prefix, element, deallocate, safe_memory)                  \
	SLL_FIRST(name, prefix, element, deallocate)                               \
	SLL_IS_ACTIVE(name, prefix, element, deallocate)                           \
	SLL_FRONT(name, prefix, element, deallocate)
#endif