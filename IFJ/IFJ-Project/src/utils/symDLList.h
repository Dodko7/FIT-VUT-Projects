/**
 * DLList pro symtable
 * Vytvořil: Lukáš Šidlík (xsidlil00)
 */

#ifndef SYMDLLIST
#define SYMDLLIST

#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @struct pro prvek dll seznamu
 */
typedef struct DLLElement{
    symtable_t *symtable;        //symbolická tabulka
    struct DLLElement *next;            //ukazatel na následujíci prvek
    struct DLLElement *prev;            //ukazatel na predchozí prvek
}*DLLElementPtr;

/**
 * @struct pro dll seznam   
 */
typedef struct DLList{
    DLLElementPtr first;                //ukazatel na prvni prvek
    DLLElementPtr current;              //ukazatel na aktivní prvek
    DLLElementPtr last;                 //ukazatel na poslední prvek
    int length;                         //délka seznamu
}DLList;

/**
 * Inicializuje seznam, tzn. nastaví všechny jeho ukazatele na NULL a délku na 0.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init(DLList *list);

/**
 * Nastaví aktuální prvek na začátek seznamu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_First(DLList *list);

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last(DLList *list);

/**
 * Vloží nový element na konec seznamu.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param symtable Ukazatel na strukturu symtable, kterou chceme vložit
 *
 * @return Vrací true, pokud se operace podařila, jinak false
 */
symtable_t * DLL_Insert_last(DLList *list);

/**
 * Vrátí první prvek seznamu, nebo NULL, pokud je seznam prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @return Prvek ze seznamu, nebo NULL
 */
symtable_t * DLL_GetFirst(DLList *list);

/**
 * Vrátí poslední prvek seznamu, nebo NULL, pokud je seznam prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @return Poslední prvek ze seznamu, nebo NULL
 */
symtable_t * DLL_GetLast(DLList *list);

/**
 * Vrátí aktuální prvek ze seznamu, nebo NULL, pokud seznam nemá žádný aktivní prvek.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @return Aktuální prvek ze seznamu, nebo NULL
 */
symtable_t * DLL_GetCurrent(DLList *list);

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns true, pokud se vše podařilo, jinak false
 */
bool DLL_Delete_last(DLList *list);

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next(DLList *list);

/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Prev(DLList *list);

/**
 * Funkce kontroluje, zda je seznam aktivní.
 * 
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 * 
 * @returns Vrací true, pokud je aktuální prvek seznamu nenulový, jinak vrací false.
 */
bool DLL_isActive(DLList *list);

/**
 * Zruší všechny prvky v seznamu a uvolní jim alokovanou paměť.
 * Pokud se při mazání jednotlivých symtable vyskytne chyba, vrací se false.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 * @return true, pokud došlo k úspěšnému zrušení všech prvků, false jinak
 */
bool DLL_Destroy(DLList *list);


#endif