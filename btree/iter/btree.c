/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h 
 * a připravených koster funkcí implementujte binární vyhledávací 
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci. 
 */
void bst_init(bst_node_t **tree) {
  *tree = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * hodnotu daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 * 
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  // test prazdneho tromu
  if (tree == NULL) 
  {
    return false; 
  }

  // iterativne prechadzame strom
  while (tree != NULL) 
  {
    if (key == tree->key) 
    { // kluc najdeny
      *value = tree->value;
      return true;
    } else if (key < tree->key) 
    { // hladame v lavom podstrome
      tree = tree->left;
    } else if (key > tree->key)
    { // hladame v pravom podstrome
      tree = tree->right;
    }
  }
  // kluc nenajdeny
  return false; 
}
/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší. 
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
  // vytvorenie noveho uzlu
  bst_node_t *new_node = malloc(sizeof(bst_node_t));

  // chyba alokacie
  if (new_node == NULL) 
  {
    exit(EXIT_FAILURE);
  }

  // inicializacia noveho uzlu
  new_node->key = key;
  new_node->value = value;
  new_node->left = NULL;
  new_node->right = NULL;

  // ak je strom prazdny, novy uzol je koren
  if (*tree == NULL) 
  {
    *tree = new_node;
    return;
  }

  // iterativne prechadzame strom
  bst_node_t *current = *tree;
  while (1) 
  {
    if (key == current->key) 
    { // uzol s danym klucom uz existuje, nahradime hodnotu
      current->value = value;
      // uvolnime alokovanu pamat pre nepotrebny uzol
      free(new_node); 
      return;
    } else if (key < current->key) 
    { // vkladam do laveho podstromu
      if (current->left == NULL) 
      { // pokial uzol neexistuje, vytvorime ho
        current->left = new_node;
        return;  
      } else 
      { // inak pokracujeme v prechode stromu
        current = current->left;
      }
    } else 
    { // vkladam do praveho podstromu
      if (current->right == NULL) 
      { // pokial uzol neexistuje, vytvorime ho
        current->right = new_node;
        return;
      } else 
      { // inak pokracujeme v prechode stromu
        current = current->right;
      }
    }
  }
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 * 
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 * 
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
  // test na target
  if (target == NULL)
  {
    return;
  }
  // iterativne prechadzame strom
  while ((*tree)->right != NULL)
  {
    // presun na praveho potomka
    tree = &((*tree)->right);
  }
  // nahradenie uzla
  target->key = (*tree)->key;
  target->value = (*tree)->value;
  bst_node_t *current = *tree;
  *tree = (*tree)->left;
  // uvolnenie pamate
  free(current);
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 * 
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 * 
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key) {
  while (*tree != NULL)
  { // kluc je lavy potomok
    if (key < (*tree)->key)
    {
      tree = &((*tree)->left);
    }
    // kluc je pravy potomok
    else if (key > (*tree)->key)
    {
      tree = &((*tree)->right);
    }
    // kluc je rovnaky
    else
    { // ak uzol nema ziadneho potomka
      if ((*tree)->left == NULL && (*tree)->right == NULL)
      {
        free(*tree);
        *tree = NULL;
      }
      // ak ma uzol potomkov
      else
      { 
        // vytvorenie pomocneho uzla
        bst_node_t *current = *tree;
        // ak ma uzol dvoch potomkov
        if ((*tree)->left != NULL && (*tree)->right != NULL)
        {
          // nahradenie uzla najpravejsim uzlom laveho podstromu
          bst_replace_by_rightmost(*tree, &((*tree)->left));
          return;
        }
        else
        { // uzol v lavom podstrome
          if ((*tree)->left != NULL)
          {
            *tree = (*tree)->left;
          }
          // uzol v pravom podstrome
          else
          {
            *tree = (*tree)->right;
          }
          // uvolnenie pomocneho uzla
          free(current);
        }
      }
    }
  }
}

/*
 * Zrušení celého stromu.
 * 
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po 
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených 
 * uzlů.
 * 
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree) {
  // inicializacia zasobnika
  stack_bst_t stack;
  stack_bst_init(&stack);

  // vlozime koren do zasobnika
  stack_bst_push(&stack, *tree);

  // iterativne uvolnujeme uzly
  while (!stack_bst_empty(&stack)) 
  {
    // ziskame uzol zo zasobnika
    bst_node_t *current = stack_bst_pop(&stack);
    if (current->left != NULL) 
    { // ak existuje lavy potomok, vlozime ho do zasobnika
      stack_bst_push(&stack, current->left);
    }
    if (current->right != NULL) 
    { // ak existuje pravy potomok, vlozime ho do zasobnika
      stack_bst_push(&stack, current->right);
    }
    free(current);
  }

  // aktualizujeme koren
  *tree = NULL;
}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit, bst_items_t *items) {
  // prechadzame strom
  while (tree != NULL) 
  {
    // zpracovanie uzlu
    stack_bst_push(to_visit, tree);  
    bst_add_node_to_items(tree, items);
    // presun na laveho potomka
    tree = tree->left;  
  }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items) {
  // inicializacia zasobnika
  stack_bst_t stack;
  stack_bst_init(&stack);

  // volame funkci pre iterativny preorder pruchod
  bst_leftmost_preorder(tree, &stack, items);

  // iterativne spracuvame uzly zo zasobnika
  while (!stack_bst_empty(&stack)) 
  {
    // ziskame uzol zo zasobnika
    tree = stack_bst_top(&stack);
    // ak existuje pravy potomok, volame funkci pre pravy podstrom
    stack_bst_pop(&stack);
    bst_leftmost_preorder(tree->right, &stack, items);
  }
}

/*
 * Pomocná funkce pro iterativní inorder.
 * 
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
  // prechadzame strom
  while (tree != NULL) 
  {
    // zpracovanie uzlu
    stack_bst_push(to_visit, tree);  
    // presun na laveho potomka
    tree = tree->left;  
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items) {
  // inicializacia zasobnika
  stack_bst_t stack;
  stack_bst_init(&stack);

  // volame funkciu pre iterativny inorder priechod
  bst_leftmost_inorder(tree, &stack);

  // iterativne spracuvame uzly zo zasobnika
  while (!stack_bst_empty(&stack)) 
  { 
    // ziskame uzol zo zasobnika
    tree = stack_bst_top(&stack);
    // zpracovanie uzlov
    stack_bst_pop(&stack);
    bst_add_node_to_items(tree, items);
    bst_leftmost_inorder(tree->right, &stack);
  }
}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit, stack_bool_t *first_visit) {
  // prechadzame strom
  while (tree != NULL) 
  { 
    // zpracovanie uzlu
    stack_bst_push(to_visit, tree);  
    stack_bool_push(first_visit, true);
    // presun na laveho potomka
    tree = tree->left;  
  }
}
 
/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items) {
  // inicializacia zasobnikov
  bool from_left;
  stack_bst_t stack;
  stack_bst_init(&stack);
  stack_bool_t first_visit;
  stack_bool_init(&first_visit);

  // volame funkci pre iterativny postorder pruchod
  bst_leftmost_postorder(tree, &stack, &first_visit);

  // iterativne spracuvame uzly zo zasobniku
  while (!stack_bst_empty(&stack)) 
  {
    // ziskame uzol zo zasobniku
    tree = stack_bst_top(&stack);
    from_left = stack_bool_top(&first_visit);
    if (from_left) 
    { // ak uzol bol navstiveny prvy krat, presunieme sa na praveho potomka
      stack_bool_pop(&first_visit);
      stack_bool_push(&first_visit, false);
      if (tree->right != NULL) 
      {
        bst_leftmost_postorder(tree->right, &stack, &first_visit);
      }
    } else 
    { // inak zpracujeme uzol
      stack_bst_pop(&stack);
      stack_bool_pop(&first_visit);
      bst_add_node_to_items(tree, items);
    }
  }
}
