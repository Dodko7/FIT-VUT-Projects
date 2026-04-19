/**
 * DLList pro symtable
 * Vytvořil: Lukáš Šidlík (xsidlil00)
 */

#include "symDLList.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void DLL_Init(DLList *list) {
    list->first = NULL;
    list->current = NULL;
    list->last = NULL;
    list->length = 0;
}

void DLL_First(DLList *list) {
    list->current = list->first;
}

void DLL_Last(DLList *list) {
    list->current = list->last;
}

void DLL_Next(DLList *list) {
    if (list->current != NULL) {
        list->current = list->current->next;
    }
}

void DLL_Prev(DLList *list) {
    if (list->current != NULL) {
        list->current = list->current->prev;
    }
}

bool DLL_isActive(DLList *list) {
    return list->current != NULL;
}

bool DLL_Destroy(DLList *list) {
    while (list->first != NULL) {
        DLLElementPtr deleteElement = list->first;
        list->first = deleteElement->next;
        if (!symtable_destroy((symtable_t *)deleteElement->symtable)) {
            return false;
        }
        free(deleteElement);
        list->length--;
    }
    return true;
}

symtable_t * DLL_Insert_last(DLList *list) {
    DLLElementPtr new = (DLLElementPtr)malloc(sizeof(struct DLLElement));
    if (new == NULL) {
        return NULL;
    }

    new->symtable  = (symtable_t *)malloc(sizeof(symtable_t));
    if (new->symtable == NULL) {
        free(new);
        return NULL;
    }

    symtable_init(new->symtable);
    new->next = NULL;
    new->prev = list->last;
    if (list->last != NULL) {
        list->last->next = new;
    } else {
        list->first = new;
    }
    list->last = new;
    list->length++;
    
    return (symtable_t *)new->symtable;
}

symtable_t *DLL_GetFirst(DLList *list) {
    if (list->first != NULL) {
        return (symtable_t *)list->first->symtable;
    }
    return NULL;
}

symtable_t *DLL_GetLast(DLList *list) {
    if (list->last != NULL) {
        return (symtable_t *)list->last->symtable;
    }
    return NULL;
}

symtable_t *DLL_GetCurrent(DLList *list) {
    if (list->current != NULL) {
        return (symtable_t *)list->current->symtable;
    }
    return NULL;
}

bool DLL_Delete_last(DLList *list) {
    DLLElementPtr delete;
    if (list->last != NULL) {
        delete = list->last;
        if (list->current == list->last) {
            list->current = NULL;
        }
        if (list->first == list->last) {
            list->first = NULL;
            list->last = NULL;
        } else {
            list->last = list->last->prev;
            list->last->next = NULL;
        }
        if (!symtable_destroy((symtable_t *)delete->symtable)) {
            return false;
        }
        free(delete);
        list->length--;
    }
    return true;
}
