#include "memory_table.h"
#include "hashtable.c"
#include "sll.c"

memory_sllist *memory_sll_init()
{
	memory_sllist *sll;
	if (0)
		sll = imalloc(sizeof(*sll));
	else
		sll = malloc(sizeof(*sll));
	if (sll == ((void *)0))
	{
		return ((void *)0);
	}
	sll->firstElement = ((void *)0);
	sll->currentLength = 0;
	return sll;
}
void memory_sll_insert(memory_sllist *sll, size_t index, void *data)
{
	memory_sll_element_ptr newElement;
	if (0)
		newElement = imalloc(sizeof(struct _memory_sll_element));
	else
		newElement = malloc(sizeof(struct _memory_sll_element));
	if (newElement == ((void *)0))
	{
		return;
	}
	if (0)
		newElement->ptr = imalloc(sizeof(void *));
	else
		newElement->ptr = malloc(sizeof(void *));
	if (newElement->ptr == ((void *)0))
	{
		return;
	}
	if (nothing != nothing)
		nothing(newElement->ptr, data);
	else
		*newElement->ptr = data;
	if (index == 0)
	{
		newElement->next = sll->firstElement;
		sll->firstElement = newElement;
	}
	else
	{
		memory_sll_element_ptr temp = memory_sll_at(sll, index - 1);
		if (temp->next != ((void *)0))
		{
			newElement->next = temp->next;
		}
		else
		{
			newElement->next = ((void *)0);
		}
		temp->next = newElement;
	}
	sll->currentLength++;
}
void memory_sll_delete(memory_sllist *sll, size_t index)
{
	if (index == 0)
	{
		memory_sll_element_ptr temp = sll->firstElement;
		sll->firstElement = temp->next;
		if (temp != ((void *)0) && temp->ptr != ((void *)0))
		{
			memt_deinit(temp->ptr);
			if (0)
			{
				if (memt_deinit == nothing)
					ifree(temp->ptr);
				ifree(temp);
			}
			else
			{
				if (memt_deinit == nothing)
					free(temp->ptr);
				free(temp);
			}
			sll->currentLength--;
		}
		return;
	}
	memory_sll_element_ptr temp = memory_sll_at(sll, index - 1);
	memory_sll_element_ptr target = ((void *)0);
	if (temp != ((void *)0) && temp->next != ((void *)0))
	{
		target = temp->next;
		temp->next = temp->next->next;
	}
	if (target != ((void *)0))
	{
		memt_deinit(target->ptr);
		if (0)
		{
			if (memt_deinit == nothing)
				free(target->ptr);
			free(target);
		}
		else
		{
			if (memt_deinit == nothing)
				free(target->ptr);
			free(target);
		}
		sll->currentLength--;
	}
}
void memory_sll_push_front(memory_sllist *sll, void *data) { memory_sll_insert(sll, 0, data); }
void memory_sll_pop_front(memory_sllist *sll) { memory_sll_delete(sll, 0); }
memory_sll_element_ptr memory_sll_at(memory_sllist *sll, size_t index)
{
	if (index >= sll->currentLength)
	{
		return ((void *)0);
	}
	memory_sll_element_ptr temp = sll->firstElement;
	for (size_t i = 0; i < index; i++)
	{
		temp = temp->next;
	}
	return temp;
}
memory_sll_element_ptr memory_sll_next(memory_sllist *sll)
{
	if (sll->firstElement == ((void *)0) || sll->activeElement == ((void *)0))
	{
		return ((void *)0);
	}
	sll->activeElement = sll->activeElement->next;
	return sll->activeElement;
}
void memory_sll_dispose(memory_sllist *sll)
{
	memory_sll_clear(sll);
	if (0)
		free(sll);
}
void memory_sll_clear(memory_sllist *sll)
{
	while (sll->firstElement != ((void *)0))
	{
		memory_sll_element_ptr temp = sll->firstElement;
		sll->firstElement = sll->firstElement->next;
		memt_deinit(temp->ptr);
		if (0)
		{
			if (memt_deinit == nothing)
				ifree(temp->ptr);
			ifree(temp);
		}
		else
		{
			if (memt_deinit == nothing)
				free(temp->ptr);
			free(temp);
		}
	}
}
void memory_sll_first(memory_sllist *sll) { sll->activeElement = sll->firstElement; }
_Bool memory_sll_is_active(memory_sllist *sll) { return sll->activeElement != ((void *)0); }
memory_sll_element_ptr memory_sll_front(memory_sllist *sll) { return sll->firstElement; }
void memory_ht_init(memory_ht *table)
{
	memory_sllist *template = memory_sll_init();
	for (int i = 0; i < 9973; i++)
	{
		((table->table)[i]) = *template;
	}
	free(template);
}
memory_sll_element_ptr memory_ht_search(memory_ht *table, void *key)
{
	size_t hash = memt_hash(key);
	memory_sllist *row = &(table->table)[hash];
	memory_sll_first(row);
	memory_sll_element_ptr item = memory_sll_front(row);
	while (memory_sll_is_active(row))
	{
		if (memt_comp == nothing_size)
		{
			if (*item->ptr == key)
				return item;
		}
		else
		{
			if (memt_comp(*item->ptr, key))
			{
				return item;
			}
		}
		item = memory_sll_next(row);
	}
	return ((void *)0);
}
void memory_ht_insert(memory_ht *table, void *data)
{
	memory_sll_element_ptr item = memory_ht_search(table, memt_key(data));
	if (item == ((void *)0))
	{
		size_t hash = memt_hash(memt_key(data));
		memory_sll_push_front(&(table->table)[hash], data);
		table->size++;
	}
	else
	{
		memt_deinit(item->ptr);
		item->ptr = memt_copy(data);
	}
}
void **memory_ht_get(memory_ht *table, void *key)
{
	memory_sll_element_ptr item = memory_ht_search(table, key);
	if (item != ((void *)0))
	{
		return item->ptr;
	}
	return ((void *)0);
}
void memory_ht_delete(memory_ht *table, void *key)
{
	size_t hash = memt_hash(key);
	memory_sllist *row = &((table->table)[hash]);
	memory_sll_first(row);
	size_t index = 0;
	memory_sll_element_ptr item = memory_sll_front(row);
	while (memory_sll_is_active(row))
	{
		if (memt_comp == nothing_size)
		{
			if (*item->ptr == key)
			{
				memory_sll_delete(row, index);
				table->size--;
				return;
			}
		}
		else
		{
			if (memt_comp(*item->ptr, key))
			{
				memory_sll_delete(row, index);
				table->size--;
				return;
			}
		}
		item = memory_sll_next(row);
		index++;
	}
}
void memory_ht_dispose(memory_ht *table)
{
	for (size_t i = 0; i < 9973; i++)
	{
		memory_sll_dispose(&(table->table)[i]);
	}
}

size_t memt_hash(void *src)
{
	return (int)src % MEMORY_TABLE_SIZE;
}

void *memt_key(void *src)
{
	return src;
}

void *memt_copy(void *src)
{
	// void **copy = malloc(sizeof(void *));
	// *copy = src;
	return src;
}
void memt_deinit(void **src)
{
	free(*src);
	free(src);
}

bool memt_comp(void *src, void *other)
{
	return src == other;
}

/**
 * @brief Alokace pameti s moznosti sledovani.
 *
 * Funkce alokuje pamet o velikosti size a vraci ukazatel na ni. Pokud je
 * zapnuta kontrola pameti (safe_memory), tak se pamet take ulozi do tabulky
 * pameti, cimz ji lze pozdeji sledovat a uvolnit.
 *
 * @param size Velikost pameti, ktera se ma alokovat.
 * @return Ukazatel na alokovanou pamet.
 */
void *imalloc(size_t size)
{
	void *ptr = malloc(size);
	if (safe_memory)
		memory_ht_insert(&_memory_table, ptr);
	return ptr;
}
/**
 * @brief Uvolni pamet alokovanou pomoci imalloc.
 *
 * Funkce uvolni pamet, ktera byla alokovana pomoci imalloc.
 * Pokud je zapnuta kontrola pameti, tak se prvne pokusime vymazat
 * prvky z tabulky pameti, cimz ji uvolnime, jinak uvolnime pamet s pomoci free.
 *
 * @param ptr Ukazatel na pamet, ktera se ma uvolnit.
 */
void ifree(void *ptr)
{
	if (safe_memory)
		memory_ht_delete(&_memory_table, ptr);
	else
		free(ptr);
}
void *irealloc(void *ptr, size_t size)
{
	void *newptr = realloc(ptr, size);
	if (safe_memory)
		memory_ht_insert(&_memory_table, newptr);
	return newptr;
}