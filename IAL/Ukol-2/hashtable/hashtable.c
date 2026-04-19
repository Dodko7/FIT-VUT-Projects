/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
} // vracia index HT prideleny klucu

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {
  for (int i = 0; i < HT_SIZE; i++) 
  {
    (*table)[i] = NULL;
  }
} // inicializuje vsetky polozky v tabulke na NULL

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
  // ziskanie indexu pre kluc
  int index = get_hash(key);
  // pomocny pointer na aktualny prvok na indexe HT
  ht_item_t *current = (*table)[index];

  // prechadzanie zoznamu na indexe HT
  while (current != NULL) 
  {
    // ak sa kluc zhoduje s aktualnym prvkom, vratime ho
    if (strcmp(current->key, key) == 0) 
    {
      return current; 
    }
    // inak prejdeme na dalsi prvok
    current = current->next;
  }
  // ak sme nenasli zhodu, vratime NULL
  return NULL; 
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
  // ziskanie indexu pre kluc
  int index = get_hash(key);
  // pomocnemu pointeru priradime prvok na indexe HT
  ht_item_t *existing_item = ht_search(table, key);

  // ak prvok s danym klucom uz existuje, nahradime jeho hodnotu
  if (existing_item != NULL)
  {
    existing_item->value = value;
  } else 
  {
    // ak neexistuje, vytvorime novy prvok
    ht_item_t *new_item = (ht_item_t *)malloc(sizeof(ht_item_t));
    if (new_item == NULL) 
    {
      exit(EXIT_FAILURE);
    }
    // priradenie hodnoty noveho prvku
    new_item->key = strdup(key); 
    new_item->value = value;
    new_item->next = NULL;
    // vlozenie noveho prvku na zaciatok zoznamu synonym
    new_item->next = (*table)[index];
    (*table)[index] = new_item;
  }
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
  // priradenie pomocnemu pointeru prvok s danym klucom
  ht_item_t *item = ht_search(table, key);

  // ak sme nasli zhodu, vratime hodnotu prvku
  if (item != NULL) 
  {
    return &(item->value);
  } else 
  {
    // ak nie, vratime NULL
    return NULL;
  }
}

/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
  // ziskanie indexu pre kluc
  int index = get_hash(key);
  // pomocny pointer na aktualny prvok na indexe HT
  ht_item_t *current = (*table)[index];
  // pomocny pointer na predchadzajuci prvok
  ht_item_t *previous = NULL;

  // prechadzanie zoznamu na indexe HT
  while (current != NULL) 
  {
    // ak sa kluc zhoduje s klucom aktualneho prvku, vymazeme ho
    if (strcmp(current->key, key) == 0) 
    {
      // ak je prvok na zaciatku zoznamu
      if (previous == NULL) 
      {
        // priradime nasledujuci prvok na indexe HT
        (*table)[index] = current->next;
      } else 
      {
        // inak priradime nasledujuci prvok predchadzajucemu prvku
        previous->next = current->next;
      }
      // uvolnime alokovane zdroje
      free(current->key);
      free(current);
      return;
    }
    // inak prejdeme na dalsi prvok
    previous = current;
    current = current->next;
  }
}

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po 
 * inicializaci.
 */
void ht_delete_all(ht_table_t *table) {
  // prechadzanie celej tabulky
  for (int i = 0; i < HT_SIZE; i++) 
  {
    // pomocny pointer na aktualny prvok na indexe HT
    ht_item_t *current = (*table)[i];
    // pomocny pointer na nasledujuci prvok
    ht_item_t *next = NULL;

    // prechadzanie zoznamu prvkov na indexe HT
    while (current != NULL) 
    {
      // priradenie nasledujuceho prvku
      next = current->next;
      // uvolnenie alokovanych zdrojov
      free(current->key);
      free(current);
      // prechod na nasledujuci prvok
      current = next;
    }
    // priradenie NULL na indexe HT
    (*table)[i] = NULL;
  }
}
