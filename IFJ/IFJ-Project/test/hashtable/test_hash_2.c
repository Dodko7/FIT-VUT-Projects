#include <stdio.h>
#include "../../src/utils/hashtable.c"
#include "../../src/utils/sll.c"

size_t getHash(void *src)
{
	return (int)src % 11;
}

void *key(void *src)
{
	return src;
}

int *ht_copy(void *src)
{
	void **copy = malloc(sizeof(void *));
	*copy = src;
	return copy;
}
void ht_deinit(void **src)
{
	free(*src);
}

bool ht_comp(void *src, void *other)
{
	return src == other;
}
_HASHTABLE(11, ht, void *, void *)

ht_sllist *ht_sll_init()
{
	ht_sllist *sll;
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
void ht_sll_insert(ht_sllist *sll, size_t index, void *data)
{
	ht_sll_element_ptr newElement;
	if (0)
		newElement = imalloc(sizeof(struct _ht_sll_element));
	else
		newElement = malloc(sizeof(struct _ht_sll_element));
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
		ht_sll_element_ptr temp = ht_sll_at(sll, index - 1);
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
void ht_sll_delete(ht_sllist *sll, size_t index)
{
	if (index == 0)
	{
		ht_sll_element_ptr temp = sll->firstElement;
		sll->firstElement = temp->next;
		if (temp != ((void *)0) && temp->ptr != ((void *)0))
		{
			nothing(temp->ptr);
			if (0)
			{
				if (nothing == nothing)
					ifree(temp->ptr);
				ifree(temp);
			}
			else
			{
				if (nothing == nothing)
					free(temp->ptr);
				free(temp);
			}
			sll->currentLength--;
		}
		return;
	}
	ht_sll_element_ptr temp = ht_sll_at(sll, index - 1);
	ht_sll_element_ptr target = ((void *)0);
	if (temp != ((void *)0) && temp->next != ((void *)0))
	{
		target = temp->next;
		temp->next = temp->next->next;
	}
	if (target != ((void *)0))
	{
		nothing(target->ptr);
		if (0)
		{
			if (nothing == nothing)
				free(target->ptr);
			free(target);
		}
		else
		{
			if (nothing == nothing)
				free(target->ptr);
			free(target);
		}
		sll->currentLength--;
	}
}
void ht_sll_push_front(ht_sllist *sll, void *data) { ht_sll_insert(sll, 0, data); }
void ht_sll_pop_front(ht_sllist *sll) { ht_sll_delete(sll, 0); }
ht_sll_element_ptr ht_sll_at(ht_sllist *sll, size_t index)
{
	if (index >= sll->currentLength)
	{
		return ((void *)0);
	}
	ht_sll_element_ptr temp = sll->firstElement;
	for (size_t i = 0; i < index; i++)
	{
		temp = temp->next;
	}
	return temp;
}
ht_sll_element_ptr ht_sll_next(ht_sllist *sll)
{
	if (sll->firstElement == ((void *)0) || sll->activeElement == ((void *)0))
	{
		return ((void *)0);
	}
	sll->activeElement = sll->activeElement->next;
	return sll->activeElement;
}
void ht_sll_dispose(ht_sllist *sll)
{
	ht_sll_clear(sll);
	if (0)
		free(sll);
}
void ht_sll_clear(ht_sllist *sll)
{
	while (sll->firstElement != ((void *)0))
	{
		ht_sll_element_ptr temp = sll->firstElement;
		sll->firstElement = sll->firstElement->next;
		nothing(temp->ptr);
		if (0)
		{
			if (nothing == nothing)
				ifree(temp->ptr);
			ifree(temp);
		}
		else
		{
			if (nothing == nothing)
				free(temp->ptr);
			free(temp);
		}
	}
}
void ht_sll_first(ht_sllist *sll) { sll->activeElement = sll->firstElement; }
_Bool ht_sll_is_active(ht_sllist *sll) { return sll->activeElement != ((void *)0); }
ht_sll_element_ptr ht_sll_front(ht_sllist *sll) { return sll->firstElement; }
void ht_ht_init(ht_ht *table)
{
	ht_sllist *template = ht_sll_init();
	for (int i = 0; i < 11; i++)
	{
		((table->table)[i]) = *template;
	}
	free(template);
}
ht_sll_element_ptr ht_ht_search(ht_ht *table, void *key)
{
	size_t hash = getHash(key);
	ht_sllist *row = &(table->table)[hash];
	ht_sll_first(row);
	ht_sll_element_ptr item = ht_sll_front(row);
	while (ht_sll_is_active(row))
	{
		if (ht_comp == nothing_size)
		{
			if (*item->ptr == key)
				return item;
		}
		else
		{
			if (ht_comp(*item->ptr, key))
			{
				return item;
			}
		}
		item = ht_sll_next(row);
	}
	return ((void *)0);
}
void ht_ht_insert(ht_ht *table, void *data)
{
	ht_sll_element_ptr item = ht_ht_search(table, key(data));
	if (item == ((void *)0))
	{
		size_t hash = getHash(key(data));
		ht_sll_push_front(&(table->table)[hash], data);
		table->size++;
	}
	else
	{
		ht_deinit(item->ptr);
		item->ptr = ht_copy(data);
	}
}
void **ht_ht_get(ht_ht *table, void *key)
{
	ht_sll_element_ptr item = ht_ht_search(table, key);
	if (item != ((void *)0))
	{
		return item->ptr;
	}
	return ((void *)0);
}
void ht_ht_delete(ht_ht *table, void *key)
{
	size_t hash = getHash(key);
	ht_sllist *row = &((table->table)[hash]);
	ht_sll_first(row);
	size_t index = 0;
	ht_sll_element_ptr item = ht_sll_front(row);
	while (ht_sll_is_active(row))
	{
		if (ht_comp == nothing_size)
		{
			if (*item->ptr == key)
			{
				ht_sll_delete(row, index);
				table->size--;
				return;
			}
		}
		else
		{
			if (ht_comp(*item->ptr, key))
			{
				ht_sll_delete(row, index);
				table->size--;
				return;
			}
		}
		item = ht_sll_next(row);
		index++;
	}
}
void ht_ht_dispose(ht_ht *table)
{
	for (size_t i = 0; i < 11; i++)
	{
		ht_sll_dispose(&(table->table)[i]);
	}
}

void ht_print_table(ht_ht *table)
{
	for (int i = 0; i < 11; i++)
	{
		printf("%i: ", i);
		int count = 0;
		ht_sll_element *item = ((table->table)[i]).firstElement;
		while (item != NULL)
		{
			printf("(%p, %3d)", item->ptr, **(int **)item->ptr);
			item = item->next;
		}
		printf("\n");
	}
}
int main()
{
	ht_ht table;
	ht_sllist *pointers = ht_sll_init();
	ht_ht_init(&table);
	void *ptr = malloc(sizeof(int));
	for (int i = 1; i <= 54; i++)
	{
		*(int *)(ptr) = i;
		ht_ht_insert(&table, ptr);
		ht_sll_push_front(pointers, ptr);
		if (*(int *)(ptr) % 2 == 0)
		{
		}
		ptr = malloc(sizeof(int));
	}

	ht_ht_delete(&table, *(ht_sll_at(pointers, 47)->ptr));
	ht_ht_delete(&table, *(ht_sll_at(pointers, 24)->ptr));
	ht_ht_delete(&table, *(ht_sll_at(pointers, 13)->ptr));

	ht_ht_delete(&table, *(ht_sll_at(pointers, 2)->ptr));
	ht_print_table(&table);
	ht_ht_dispose(&table);
	return 0;
}