#include <stdio.h>
#include "../../src/utils/hashtable.c"
#include "../../src/utils/sll.c"

size_t getHash(int src)
{
    return src % 11;
}

int key(int src){
	return src;
}

int *ht_copy(int src){
	int *copy = malloc(sizeof(int));
	*copy = src;
	return copy;
}
void ht_deinit(int *src){
	free(src);
}

size_t ht_comp(int src, int other){
	return src == other;
}


_HASHTABLE(11, ht, int, int)
SLL(test, ht, int, nothing, nothing, false, true)
HASHTABLE(11, ht, int, int, getHash, key, ht_copy, ht_comp, ht_deinit)

void ht_print_table(ht_ht *table) {
  for (int i = 0; i < 11; i++) {
    printf("%i: ", i);
	ht_sll_element *item = ((table->table)[i]).firstElement;
    while (item != NULL) {
      printf("_%d_", *item->ptr);
      item = item->next;
    }
  }
  printf("\n");
}
int main() {
	ht_ht table;
	ht_ht_init(&table);
	for(int i = 0; i < 39; i++){
		ht_ht_insert(&table, i);	
	}
	ht_print_table(&table);
	ht_ht_dispose(&table);
	return 0;
}