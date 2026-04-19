#include <stdlib.h>
#include "memory_table.h"
/**
 * @file dll.c
 * @brief Dynamicky alokovany spojovy seznam
 *
 * Modul obsahuje funkci pro inicializaci seznamu, vkladani a odebrani prvku
 * a funkcionality pro praci s aktivnim prvkem.
 *
 * @author xramas01 Jakub Ramaseuski
 * @cite xsidlil00 Lukas Sidlik
 */
#include "dll.h"
#ifndef DLL_C
#define DLL_C

#ifdef DLL_TRAD

/**
 * @brief Inicializuje seznam
 *
 * Funkce inicializuje dvousměrně vázaný seznam. Nastaví všechny ukazatele
 * na NULL a délku na 0.
 *
 * @param dll Ukazatel na strukturu seznamu, který se má inicializovat.
 */
#define DLL_INIT(name, prefix, element, deallocate) \
	void prefix##_dll_init(prefix##_dllist *dll)    \
	{                                               \
		dll->firstElement = NULL;                   \
		dll->activeElement = NULL;                  \
		dll->lastElement = NULL;                    \
		dll->currentLength = 0;                     \
	}
#else

/**
 * @brief Inicializace dynamicky alokovaneho seznamu
 *
 * Funkce inicializuje dynamicky alokovany seznam. Vraci ukazatel na
 * strukturu seznamu, nebo NULL, pokud selhalo alokovani pameti.
 *
 * @return Ukazatel na inicializovanou strukturu seznamu, nebo NULL.
 */
#define DLL_INIT(name, prefix, element, deallocate)  \
	prefix##_dllist *prefix##_dll_init()             \
	{                                                \
		prefix##_dllist *dll = imalloc(sizeof(*dll)); \
		if (dll == NULL)                             \
		{                                            \
			return NULL;                             \
		}                                            \
		dll->firstElement = NULL;                    \
		dll->lastElement = NULL;                     \
		dll->currentLength = 0;                      \
		return dll;                                  \
	}
#endif

#ifdef DLL_TRAD
/**
 * @brief Vymaze vsechny prvky seznamu
 *
 * Funkce vymaze vsechny prvky seznamu a uvolni pamet.
 */
#define DLL_CLEAR(name, prefix, element, deallocate)           \
	bool prefix##_dll_clear(prefix##_dllist *dll)              \
	{                                                          \
		while (dll->firstElement != NULL)                      \
		{                                                      \
			prefix##_dll_element_ptr temp = dll->firstElement; \
			dll->firstElement = dll->firstElement->next;       \
			deallocate(temp->ptr);                             \
			if (deallocate == nothing)                         \
				ifree(temp->ptr);                               \
			ifree(temp);                                        \
		}                                                      \
		return true;                                           \
	}

#else
/**
 * @brief Vymaze vsechny prvky seznamu
 *
 * Funkce vymaze vsechny prvky seznamu a uvolni pamet.
 */
#define DLL_CLEAR(name, prefix, element, deallocate)           \
	void prefix##_dll_clear(prefix##_dllist *dll)              \
	{                                                          \
		while (dll->firstElement != NULL)                      \
		{                                                      \
			prefix##_dll_element_ptr temp = dll->firstElement; \
			dll->firstElement = dll->firstElement->next;       \
			deallocate(temp->ptr);                             \
			if (deallocate == nothing)                         \
				ifree(temp->ptr);                               \
			ifree(temp);                                        \
		}                                                      \
	}
#endif

#ifdef DLL_TRAD
#define DLL_DISPOSE(name, prefix, element, deallocate) \
	bool prefix##_dll_dispose(prefix##_dllist *dll)    \
	{                                                  \
		bool cleared = prefix##_dll_clear(dll);        \
		if (!cleared)                                  \
			return false;                              \
		ifree(dll);                                     \
		return true;                                   \
	}
#else
/**
 * @brief Uvolni pamet alokovanou pro seznam
 *
 * Funkce uvolni pamet alokovanou pro seznam a vsechny jeho prvky.
 */
#define DLL_DISPOSE(name, prefix, element, deallocate) \
	void prefix##_dll_dispose(prefix##_dllist *dll)    \
	{                                                  \
		prefix##_dll_clear(dll);                       \
		ifree(dll);                                     \
	}
#endif
#ifdef DLL_TRAD
#define DLL_INSERT(name, prefix, element, copy, init, deallocate)                             \
	bool prefix##_dll_insert(prefix##_dllist *dll, size_t index, element data)                \
	{                                                                                         \
		prefix##_dll_element_ptr newElement = imalloc(sizeof(struct _##prefix##_dll_element)); \
		if (newElement == NULL)                                                               \
		{                                                                                     \
			return false;                                                                     \
		}                                                                                     \
		if (init == nothing)                                                                  \
			newElement->ptr = imalloc(sizeof(element));                                        \
		else                                                                                  \
			init(&newElement->ptr);                                                           \
		if (newElement->ptr == NULL)                                                          \
		{                                                                                     \
			return false;                                                                     \
		}                                                                                     \
		if (copy != nothing)                                                                  \
			copy(newElement->ptr, data);                                                      \
		else                                                                                  \
			*newElement->ptr = data;                                                          \
		if (index == 0)                                                                       \
		{                                                                                     \
			newElement->previous = NULL;                                                      \
			newElement->next = dll->firstElement;                                             \
			if (dll->firstElement != NULL)                                                    \
			{                                                                                 \
				dll->firstElement->previous = newElement;                                     \
			}                                                                                 \
			else                                                                              \
			{                                                                                 \
				dll->lastElement = newElement;                                                \
			}                                                                                 \
			dll->firstElement = newElement;                                                   \
		}                                                                                     \
		else                                                                                  \
		{                                                                                     \
			prefix##_dll_element_ptr temp = prefix##_dll_at(dll, index - 1);                  \
			newElement->previous = temp;                                                      \
			newElement->next = temp->next;                                                    \
			if (temp->next != NULL)                                                           \
			{                                                                                 \
				temp->next->previous = newElement;                                            \
			}                                                                                 \
			else                                                                              \
			{                                                                                 \
				dll->lastElement = newElement;                                                \
			}                                                                                 \
			temp->next = newElement;                                                          \
		}                                                                                     \
		dll->currentLength++;                                                                 \
		return true;                                                                          \
	}
#else
/**
 * @brief Vlozeni prvku do seznamu na pozici index
 *
 * Funkce vlozi prvek s hodnotou data do seznamu na pozici index.
 * Alokujem pamet pro novy prvek, protoze funkci je dano, ze data jsou validni.
 * V pripade, ze selhalo alokovani pameti, funkce nic nedela.
 *
 * @param dll Ukazatel na seznam, do ktereho se ma prvek vlozit.
 * @param index Pozice, na kterou se ma prvek vlozit.
 * @param data Hodnota prvku, ktery se ma vlozit.
 */
#define DLL_INSERT(name, prefix, element, copy, init, deallocate)                             \
	void prefix##_dll_insert(prefix##_dllist *dll, size_t index, element data)                \
	{                                                                                         \
		prefix##_dll_element_ptr newElement = imalloc(sizeof(struct _##prefix##_dll_element)); \
		if (newElement == NULL)                                                               \
		{                                                                                     \
			return;                                                                           \
		}                                                                                     \
		if (init == nothing)                                                                  \
			newElement->ptr = imalloc(sizeof(element));                                        \
		else                                                                                  \
			init(&newElement->ptr);                                                           \
		if (newElement->ptr == NULL)                                                          \
		{                                                                                     \
			return;                                                                           \
		}                                                                                     \
		if (copy != nothing)                                                                  \
			copy(newElement->ptr, data);                                                      \
		else                                                                                  \
			*newElement->ptr = data;                                                          \
		if (index == 0)                                                                       \
		{                                                                                     \
			newElement->previous = NULL;                                                      \
			newElement->next = dll->firstElement;                                             \
			if (dll->firstElement != NULL)                                                    \
			{                                                                                 \
				dll->firstElement->previous = newElement;                                     \
			}                                                                                 \
			else                                                                              \
			{                                                                                 \
				dll->lastElement = newElement;                                                \
			}                                                                                 \
			dll->firstElement = newElement;                                                   \
		}                                                                                     \
		else                                                                                  \
		{                                                                                     \
			prefix##_dll_element_ptr temp = prefix##_dll_at(dll, index - 1);                  \
			newElement->previous = temp;                                                      \
			newElement->next = temp->next;                                                    \
			if (temp->next != NULL)                                                           \
			{                                                                                 \
				temp->next->previous = newElement;                                            \
			}                                                                                 \
			else                                                                              \
			{                                                                                 \
				dll->lastElement = newElement;                                                \
			}                                                                                 \
			temp->next = newElement;                                                          \
		}                                                                                     \
		dll->currentLength++;                                                                 \
	}
#endif
#ifdef DLL_TRAD
#define DLL_DELETE(name, prefix, element, deallocate)                \
	bool prefix##_dll_delete(prefix##_dllist *dll, size_t index)     \
	{                                                                \
		prefix##_dll_element_ptr temp = prefix##_dll_at(dll, index); \
		if (temp == NULL)                                            \
		{                                                            \
			return false;                                            \
		}                                                            \
		if (temp->previous != NULL)                                  \
		{                                                            \
			temp->previous->next = temp->next;                       \
		}                                                            \
		else                                                         \
		{                                                            \
			dll->firstElement = temp->next;                          \
		}                                                            \
		if (temp->next != NULL)                                      \
		{                                                            \
			temp->next->previous = temp->previous;                   \
		}                                                            \
		else                                                         \
		{                                                            \
			dll->lastElement = temp->previous;                       \
		}                                                            \
		deallocate(temp->ptr);                                       \
		if (deallocate == nothing)                                   \
			ifree(temp->ptr);                                         \
		ifree(temp);                                                  \
		dll->currentLength--;                                        \
		return true;                                                 \
	}
#else
/**
 * @brief Odstrani prvek na pozici index
 *
 * Funkce odstrani prvek na pozici index v seznamu. Pokud je seznam
 * prazdny, nebo je pozice mimo rozsah, funkce nic nedela.
 *
 * @param dll Ukazatel na seznam, ze ktereho se ma prvek odstranit.
 * @param index Pozice, na ktere se ma prvek odstranit.
 */
#define DLL_DELETE(name, prefix, element, deallocate)                \
	void prefix##_dll_delete(prefix##_dllist *dll, size_t index)     \
	{                                                                \
		prefix##_dll_element_ptr temp = prefix##_dll_at(dll, index); \
		if (temp == NULL)                                            \
		{                                                            \
			return;                                                  \
		}                                                            \
		if (temp->previous != NULL)                                  \
		{                                                            \
			temp->previous->next = temp->next;                       \
		}                                                            \
		else                                                         \
		{                                                            \
			dll->firstElement = temp->next;                          \
		}                                                            \
		if (temp->next != NULL)                                      \
		{                                                            \
			temp->next->previous = temp->previous;                   \
		}                                                            \
		else                                                         \
		{                                                            \
			dll->lastElement = temp->previous;                       \
		}                                                            \
		deallocate(temp->ptr);                                       \
		if (deallocate == nothing)                                   \
			ifree(temp->ptr);                                         \
		ifree(temp);                                                  \
		dll->currentLength--;                                        \
	}
#endif

#ifdef DLL_TRAD
#define DLL_PUSH_BACK(name, prefix, element, deallocate)            \
	bool prefix##_dll_push_back(prefix##_dllist *dll, element data) \
	{                                                               \
		return prefix##_dll_insert(dll, dll->currentLength, data);  \
	}
#else

/**
 * @brief Prida prvek na konec seznamu
 *
 * Funkce prida prvek s hodnotou data na konec seznamu.
 *
 * @param dll Ukazatel na seznam, do ktereho se ma prvek vlozit.
 * @param data Hodnota prvku, ktery se ma vlozit.
 */
#define DLL_PUSH_BACK(name, prefix, element, deallocate)            \
	void prefix##_dll_push_back(prefix##_dllist *dll, element data) \
	{                                                               \
		prefix##_dll_insert(dll, dll->currentLength, data);         \
	}

#endif

/**
 * @brief Nastavi aktivni prvek na prvni prvek v seznamu
 *
 * Funkce nastavi aktivni prvek na prvni prvek v seznamu.
 *
 * @param dll Ukazatel na seznam, v nemz se nastavuje aktivni prvek.
 */
#define DLL_FIRST(name, prefix, element, deallocate) \
	void prefix##_dll_first(prefix##_dllist *dll)    \
	{                                                \
		dll->activeElement = dll->firstElement;      \
	}
#ifdef DLL_TRAD
#define DLL_PUSH_FRONT(name, prefix, element, deallocate)            \
	bool prefix##_dll_push_front(prefix##_dllist *dll, element data) \
	{                                                                \
		return prefix##_dll_insert(dll, 0, data);                    \
	}
#else
/**
 * @brief Prida prvek na zacatek seznamu
 *
 * Funkce prida prvek s hodnotou data na zacatek seznamu.
 *
 * @param dll Ukazatel na seznam, do ktereho se ma prvek vlozit.
 * @param data Hodnota prvku, ktery se ma vlozit.
 */
#define DLL_PUSH_FRONT(name, prefix, element, deallocate)            \
	void prefix##_dll_push_front(prefix##_dllist *dll, element data) \
	{                                                                \
		prefix##_dll_insert(dll, 0, data);                           \
	}
#endif
#ifdef DLL_TRAD
/**
 * @brief Odebere posledni prvek ze seznamu
 *
 * Funkce odebere posledni prvek ze seznamu. Pokud je seznam prazdny,
 * vraci false.
 *
 * @param dll Ukazatel na seznam, ze ktereho se ma prvek odebrat.
 * @return true, pokud se podarilo odebrat prvek, jinak false.
 */
#define DLL_POP_BACK(name, prefix, element, deallocate)          \
	bool prefix##_dll_pop_back(prefix##_dllist *dll)             \
	{                                                            \
		return prefix##_dll_delete(dll, dll->currentLength - 1); \
	}
#else
/**
 * @brief odstran posledni prvek ze seznamu
 *
 * Funkce odstrani posledni prvek ze seznamu.
 *
 * @param dll Ukazatel na seznam, ze ktereho se ma prvek odebrat.
 */
#define DLL_POP_BACK(name, prefix, element, deallocate)   \
	void prefix##_dll_pop_back(prefix##_dllist *dll)      \
	{                                                     \
		prefix##_dll_delete(dll, dll->currentLength - 1); \
	}
#endif

#ifdef DLL_TRAD
#define DLL_POP_FRONT(name, prefix, element, deallocate) \
	bool prefix##_dll_pop_front(prefix##_dllist *dll)    \
	{                                                    \
		return prefix##_dll_delete(dll, 0);              \
	}
#else
/**
 * @brief Odstrani prvni prvek z  seznamu
 *
 * Funkce odstrani prvni prvek z  seznamu.
 *
 * @param dll Ukazatel na seznam, ze ktereho se ma prvek odebrat.
 */
#define DLL_POP_FRONT(name, prefix, element, deallocate) \
	void prefix##_dll_pop_front(prefix##_dllist *dll)    \
	{                                                    \
		prefix##_dll_delete(dll, 0);                     \
	}
#endif
/**
 * @brief Vrati ukazatel na prvek seznamu na pozici index
 *
 * Funkce vrati ukazatel na prvek seznamu, ktery se nachazi na pozici index.
 *
 * @param dll Ukazatel na seznam, v nemz se hleda prvek.
 * @param index Pozice, na ktere se hleda prvek.
 * @return Ukazatel na prvek nebo NULL, pokud je seznam prazdny nebo je pozice mimo rozsah.
 */
#define DLL_AT(name, prefix, element, deallocate)                                \
	prefix##_dll_element_ptr prefix##_dll_at(prefix##_dllist *dll, size_t index) \
	{                                                                            \
		if (index >= dll->currentLength)                                         \
		{                                                                        \
			return NULL;                                                         \
		}                                                                        \
		prefix##_dll_element_ptr temp = dll->firstElement;                       \
		for (size_t i = 0; i < index; i++)                                       \
		{                                                                        \
			temp = temp->next;                                                   \
		}                                                                        \
		return temp;                                                             \
	}
#ifdef DLL_TRAD
#define DLL_NEXT(name, prefix, element, deallocate)        \
	void prefix##_dll_next(prefix##_dllist *dll)           \
	{                                                      \
		if (prefix##_dll_is_active(dll))                   \
		{                                                  \
			dll->activeElement = dll->activeElement->next; \
		}                                                  \
	}
#else
/**
 * @brief Vrati dalsi prvek v seznamu a posune aktivni prvek na dalsi pozici
 *
 * Funkce vrati dalsi prvek v seznamu, posune aktivni prvek na dalsi pozici a vrati jej.
 * Pokud je seznam prazdny nebo je aktivni prvek posledni v seznamu, vrati NULL.
 *
 * @param dll Ukazatel na seznam, v nemz se hleda dalsi prvek.
 * @return Ukazatel na dalsi prvek nebo NULL, pokud je seznam prazdny nebo je aktivni
 * prvek posledni v seznamu.
 */
#define DLL_NEXT(name, prefix, element, deallocate)                  \
	prefix##_dll_element_ptr prefix##_dll_next(prefix##_dllist *dll) \
	{                                                                \
		if (dll->lastElement == NULL)                                \
		{                                                            \
			return NULL;                                             \
		}                                                            \
		dll->activeElement = dll->activeElement->next;               \
		return dll->activeElement;                                   \
	}
#endif

/**
 * @brief Posune aktivni prvek na posledni pozici v seznamu
 *
 * Funkce posune aktivni prvek na posledni pozici v seznamu.
 *
 * @param dll Ukazatel na seznam, v nemz se posunuje aktivni prvek.
 */
#define DLL_LAST(name, prefix, element, deallocate) \
	void prefix##_dll_last(prefix##_dllist *dll)    \
	{                                               \
		dll->activeElement = dll->lastElement;      \
	}

/**
 * @brief Vrati true, pokud je seznam aktivni
 *
 * Funkce vrati true, pokud je seznam aktivni (tj. ma aktivni prvek).
 * V opacnem pripade vrati false.
 *
 * @param dll Ukazatel na seznam, o nemz se ptame.
 * @return True, pokud je seznam aktivni, jinak false.
 */
#define DLL_IS_ACTIVE(name, prefix, element, deallocate) \
	bool prefix##_dll_is_active(prefix##_dllist *dll)    \
	{                                                    \
		return dll->activeElement != NULL;               \
	}
/**
 * @brief Vrati prvni prvek v seznamu
 *
 * Funkce vrati prvni prvek v seznamu.
 *
 * @param dll Ukazatel na seznam, ze ktereho se hleda prvek.
 * @return Ukazatel na prvek nebo NULL, pokud je seznam prazdny.
 */
#define DLL_FRONT(name, prefix, element, deallocate)                  \
	prefix##_dll_element_ptr prefix##_dll_front(prefix##_dllist *dll) \
	{                                                                 \
		return dll->firstElement;                                     \
	}
/**
 * @brief Vrati posledni prvek v seznamu
 *
 * Funkce vrati posledni prvek v seznamu.
 *
 * @param dll Ukazatel na seznam, ze ktereho se hleda prvek.
 * @return Ukazatel na prvek nebo NULL, pokud je seznam prazdny.
 */
#define DLL_BACK(name, prefix, element, deallocate)                  \
	prefix##_dll_element_ptr prefix##_dll_back(prefix##_dllist *dll) \
	{                                                                \
		return dll->lastElement;                                     \
	}
#ifdef DLL_TRAD
#define DLL_PREVIOUS(name, prefix, element, deallocate)        \
	void prefix##_dll_previous(prefix##_dllist *dll)           \
	{                                                          \
		if (prefix##_dll_is_active(dll))                       \
		{                                                      \
			dll->activeElement = dll->activeElement->previous; \
		}                                                      \
	}
#else

#define DLL_PREVIOUS(name, prefix, element, deallocate)                  \
	prefix##_dll_element_ptr prefix##_dll_previous(prefix##_dllist *dll) \
	{                                                                    \
		if (prefix##_dll_is_active(dll))                                 \
		{                                                                \
			dll->activeElement = dll->activeElement->previous;           \
			return dll->activeElement;                                   \
		}                                                                \
	}
#endif

/**
 * @def DLL(name, prefix, element, copy, deallocate)
 *
 * @brief Makro pro generovani funkcionalit pro dvojnasobne vazany seznam
 *
 * @param prefix Prefix, ktery se ma pouzit u funkci
 * @param element Typ prvku v seznamu
 * @param copy Funkce pro kopirovani prvku
 * @param deallocate Funkce pro uvolneni pameti prvku
 */
#define DLL(name, prefix, element, copy, init, deallocate)    \
	DLL_INIT(name, prefix, element, deallocate)               \
	DLL_INSERT(name, prefix, element, copy, init, deallocate) \
	DLL_DELETE(name, prefix, element, deallocate)             \
	DLL_PUSH_BACK(name, prefix, element, deallocate)          \
	DLL_PUSH_FRONT(name, prefix, element, deallocate)         \
	DLL_POP_BACK(name, prefix, element, deallocate)           \
	DLL_POP_FRONT(name, prefix, element, deallocate)          \
	DLL_AT(name, prefix, element, deallocate)                 \
	DLL_NEXT(name, prefix, element, deallocate)               \
	DLL_DISPOSE(name, prefix, element, deallocate)            \
	DLL_CLEAR(name, prefix, element, deallocate)              \
	DLL_FIRST(name, prefix, element, deallocate)              \
	DLL_LAST(name, prefix, element, deallocate)               \
	DLL_IS_ACTIVE(name, prefix, element, deallocate)          \
	DLL_FRONT(name, prefix, element, deallocate)              \
	DLL_BACK(name, prefix, element, deallocate)               \
	DLL_PREVIOUS(name, prefix, element, deallocate)

#endif