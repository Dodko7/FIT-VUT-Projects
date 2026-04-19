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

SLL(test, ht, void *, nothing, nothing, false, false)
HASHTABLE(11, ht, void *, void *, getHash, key, ht_copy, ht_comp, ht_deinit)

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
	ht_ht_init(&table);
	void *ptr = malloc(sizeof(int));
	for (int i = 1; i <= 88; i++)
	{
		*(int *)(ptr) = i;
		ht_ht_insert(&table, ptr);
		ptr = malloc(sizeof(int));
	}
	ht_ht_delete(&table, ptr);
	ht_print_table(&table);
	ht_ht_dispose(&table);
	return 0;
}