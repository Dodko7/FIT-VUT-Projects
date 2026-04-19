/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
 * uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"
#include <stdio.h>

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
	if (list == NULL)			// kontrola, či zoznam nie je inicializovaný
	{
		DLL_Error();
		return;
	}
	
	list->activeElement = NULL;	// nastaví aktívny prvok na NULL
	list->lastElement = NULL;	// nastaví posledný prvok na NULL
	list->firstElement = NULL;	// nastaví prvý prvok na NULL
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {
    DLLElementPtr currentElement = list->firstElement;	// inicializuje pomocný ukazateľ na prvý prvok zoznamu
    DLLElementPtr nextElement;							

    while (currentElement != NULL)						// cyklus na prechádzanie prvkov zoznamu
	{
        nextElement = currentElement->nextElement;		// uloží ukazovateľ na nasledujúci prvok
        free(currentElement);							// uvoľnenie pamäte aktuálneho prvku
        currentElement = nextElement;					// nastavenie aktuálneho prvku na nasledujúci prvok
    }

	// po uvoľnení všetkých prvkov inicializuje ukazovatele na NULL
    list->firstElement = NULL;
    list->lastElement = NULL;
    list->activeElement = NULL;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, int data ) {
	// vytvorí nový prvok a alokuje preň pamäť
    DLLElementPtr newElement = (DLLElementPtr) malloc(sizeof(struct DLLElement));
    
    if (newElement == NULL)										// kontrola úspešnej alokácie pamäte
	{
        DLL_Error();
		return;
    } else 
	{
        newElement->data = data;								// nastaví hodnotu nového prvku
        newElement->previousElement = NULL; 					// nastaví ukazateľ na predchádzajúci prvok na NULL

        if (list->firstElement == NULL)							// kontrola, či je zoznam prázdny
		{
            newElement->nextElement = NULL;						// nastaví ukazateľ na nasledujúci prvok na NULL
            list->lastElement = newElement;						// nastaví ukazateľ na posledný prvok na nový prvok
        } else 
		{
            newElement->nextElement = list->firstElement;		// nastaví ukazateľ na nasledujúci prvok na prvý prvok
            list->firstElement->previousElement = newElement;	// nastaví ukazateľ na predchádzajúci prvok na nový prvok
        }
        list->firstElement = newElement;						// nastaví ukazateľ na prvý prvok na nový prvok
    }
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, int data ) {
	// vytvorí nový prvok a alokuje preň pamäť
    DLLElementPtr newElement = (DLLElementPtr) malloc(sizeof(struct DLLElement));
    
    if (newElement == NULL)										// kontrola úspešnej alokácie pamäte
	{
        DLL_Error();
		return;
    } else 
	{
        newElement->data = data;								// nastaví hodnotu nového prvku
        newElement->nextElement = NULL;							// nastaví ukazateľ na nasledujúci prvok na NULL

        if (list->firstElement == NULL)							// kontrola, či je zoznam prázdny
		{
            newElement->previousElement = NULL;					// nastaví ukazateľ na predchádzajúci prvok na NULL
            list->firstElement = newElement;					// nastaví ukazateľ na prvý prvok na nový prvok
        } else 
		{
            newElement->previousElement = list->lastElement;	// nastaví ukazateľ na predchádzajúci prvok na posledný prvok
            list->lastElement->nextElement = newElement;		// nastaví ukazateľ na nasledujúci prvok na nový prvok
        }
        list->lastElement = newElement;							// nastaví ukazateľ na posledný prvok na nový prvok
    }
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {	
	list->activeElement = list->firstElement;	// nastaví aktívny prvok na prvý prvok
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {
	list->activeElement = list->lastElement;	// nastaví aktívny prvok na posledný prvok
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, int *dataPtr ) {
    if (list->firstElement != NULL) 			// kontrola, či je zoznam prázdny
	{
        *dataPtr = list->firstElement->data;	// nastaví hodnotu prvku na hodnotu prvého prvku
    } else 
	{
        DLL_Error();	
		return;
    }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, int *dataPtr ) {
    if (list->lastElement != NULL)				// kontrola, či je zoznam prázdny
	{
        *dataPtr = list->lastElement->data;		// nastaví hodnotu prvku na hodnotu posledného prvku
    } else 
	{
        DLL_Error(); 
		return;
    }
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {
    if (list->firstElement != NULL)							// kontrola, či je zoznam prázdny
	{
        DLLElementPtr elementToDelete = list->firstElement;	// inicializuje pomocný ukazateľ na prvý prvok zoznamu
        list->firstElement = elementToDelete->nextElement;	// nastaví prvý prvok na nasledujúci prvok

        if (list->firstElement != NULL)						// kontrola, či je zoznam prázdny
		{
            list->firstElement->previousElement = NULL;		// nastaví ukazateľ na predchádzajúci prvok na NULL
        } else 
		{
            list->lastElement = NULL;						// nastaví posledný prvok na NULL
        }

        if (list->activeElement == elementToDelete)			// kontrola, či je prvok aktívny
		{
            list->activeElement = NULL;						// nastaví aktívny prvok na NULL
        }

        free(elementToDelete);								// uvoľní pamäť prvku
    } else
	{
		return;												// ak je zoznam prázdny, nič sa nedeje
	}
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {
    if (list->lastElement != NULL)							// kontrola, či je zoznam prázdny
	{
        DLLElementPtr elementToDelete = list->lastElement;	// inicializuje pomocný ukazateľ na posledný prvok zoznamu

        if (list->activeElement == elementToDelete)			// kontrola, či je prvok aktívny
		{
            list->activeElement = NULL;						// nastaví aktívny prvok na NULL
        }

        if (list->firstElement == list->lastElement)		// kontrola, či je prvý prvok aj posledný prvok
		{
            list->firstElement = NULL;						// nastaví prvý prvok na NULL
            list->lastElement = NULL;						// nastaví posledný prvok na NULL
        } else 
		{
            list->lastElement = elementToDelete->previousElement;	// nastaví posledný prvok na predchádzajúci prvok
            list->lastElement->nextElement = NULL;			// nastaví ukazateľ na nasledujúci prvok na NULL
        }

        free(elementToDelete);								// uvoľní pamäť prvku
    } else
	{
		return;												// ak je zoznam prázdny, nič sa nedeje
	}
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {
	// kontrola, či je zoznam prázdny alebo či je prvok aktívny alebo či je prvok posledný
    if (list->activeElement != NULL && list->activeElement->nextElement != NULL)
	{
        DLLElementPtr elementToDelete = list->activeElement->nextElement;			// inicializuje pomocný ukazateľ na prvok za aktívnym prvkom
        list->activeElement->nextElement = elementToDelete->nextElement;			// nastaví ukazateľ na nasledujúci prvok na prvok za prvkom za aktívnym prvkom

        if (elementToDelete == list->lastElement)									// kontrola, či je prvok posledný
		{
            list->lastElement = list->activeElement;								// nastaví posledný prvok na aktívny prvok
        } else 
		{
            elementToDelete->nextElement->previousElement = list->activeElement;	// nastaví ukazateľ na predchádzajúci prvok na aktívny prvok
        }
        free(elementToDelete);														// uvoľní pamäť prvku
    } else
	{
		return;		// ak je zoznam prázdny alebo prvok nie je aktívny alebo prvok je posledný, nič sa nedeje
	}
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {
	// kontrola, či je zoznam prázdny alebo či je prvok aktívny alebo či je prvok prvý
    if (list->activeElement != NULL && list->activeElement->previousElement != NULL) 
	{
        DLLElementPtr elementToDelete = list->activeElement->previousElement;			// inicializuje pomocný ukazateľ na prvok pred aktívnym prvkom

        if (elementToDelete == list->firstElement)										// kontrola, či je prvok prvý
		{
            list->firstElement = list->activeElement;									// nastaví prvý prvok na aktívny prvok
            list->activeElement->previousElement = NULL;								// nastaví ukazateľ na predchádzajúci prvok na NULL
        } else 
		{
            list->activeElement->previousElement = elementToDelete->previousElement;	// nastaví ukazateľ na predchádzajúci prvok na prvok pred prvkom pred aktívnym prvkom
            elementToDelete->previousElement->nextElement = list->activeElement;		// nastaví ukazateľ na nasledujúci prvok na aktívny prvok
        }
        free(elementToDelete);															// uvoľní pamäť prvku
    } else
	{
		return;		// ak je zoznam prázdny alebo prvok nie je aktívny alebo prvok je prvý, nič sa nedeje
	}
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, int data ) {	
    if (list->activeElement != NULL)										// kontrola, či je zoznam prázdny
	{
        DLLElementPtr newElement = malloc(sizeof(struct DLLElement));		// inicializuje pomocný ukazateľ na nový prvok

        if (newElement == NULL)												// kontrola, či sa podarilo alokovať pamäť
		{
            DLL_Error(); 
            return;
        }

        newElement->data = data;											// nastaví hodnotu nového prvku
        newElement->nextElement = list->activeElement->nextElement;			// nastaví ukazateľ na nasledujúci prvok na prvok za aktívnym prvkom
        newElement->previousElement = list->activeElement;					// nastaví ukazateľ na predchádzajúci prvok na aktívny prvok
        
		if (list->activeElement->nextElement != NULL)						// kontrola, či je prvok posledný
		{
            list->activeElement->nextElement->previousElement = newElement;	// nastaví ukazateľ na predchádzajúci prvok na nový prvok
        } else 
		{
            list->lastElement = newElement;									// nastaví posledný prvok na nový prvok
        }
        list->activeElement->nextElement = newElement;						// nastaví ukazateľ na nasledujúci prvok na nový prvok
    } else 
	{
        return;																// ak je prvok neaktívny, nič sa nedeje
    }
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, int data ) {
    if (list->activeElement != NULL)										// kontrola, či je zoznam prázdny
	{
        DLLElementPtr newElement = malloc(sizeof(struct DLLElement));		// inicializuje pomocný ukazateľ na nový prvok

        if (newElement == NULL)												// kontrola, či sa podarilo alokovať pamäť
		{
            DLL_Error(); 
            return;
        }

        newElement->data = data;											// nastaví hodnotu nového prvku
        newElement->nextElement = list->activeElement;						// nastaví ukazateľ na nasledujúci prvok na aktívny prvok
        newElement->previousElement = list->activeElement->previousElement;	// nastaví ukazateľ na predchádzajúci prvok na prvok pred aktívnym prvkom

        if (list->activeElement->previousElement != NULL)					// kontrola, či je prvok prvý
		{
            list->activeElement->previousElement->nextElement = newElement;	// nastaví ukazateľ na nasledujúci prvok na nový prvok
        } else 
		{
            list->firstElement = newElement;								// nastaví prvý prvok na nový prvok
        }
        list->activeElement->previousElement = newElement;					// nastaví ukazateľ na predchádzajúci prvok na nový prvok
    } else 
	{
        return;																// ak je prvok neaktívny, nič sa nedeje
    }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, int *dataPtr ) {
    if (list->activeElement != NULL)			// kontrola, či je zoznam prázdny
	{
        *dataPtr = list->activeElement->data;	// nastaví hodnotu prvku na hodnotu aktívneho prvku
    } else 
	{
        DLL_Error(); 
		return;
	}
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, int data ) {
	if (list->activeElement != NULL)		// kontrola, či je zoznam prázdny
	{
		list->activeElement->data = data;	// nastaví hodnotu aktívneho prvku na novú hodnotu
	}
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {
    if (list->activeElement != NULL)								// kontrola, či je zoznam prázdny
	{
        if (list->activeElement == list->lastElement)				// kontrola, či je prvok posledný
		{
            list->activeElement = NULL;								// nastaví aktívny prvok na NULL
        } else 
		{
            list->activeElement = list->activeElement->nextElement;	// nastaví aktívny prvok na nasledujúci prvok
        }
    }
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {	
    if (list->activeElement != NULL)									// kontrola, či je zoznam prázdny
	{
        if (list->activeElement == list->firstElement)					// kontrola, či je prvok prvý
		{
            list->activeElement = NULL;									// nastaví aktívny prvok na NULL
        } else 
		{
            list->activeElement = list->activeElement->previousElement;	// nastaví aktívny prvok na predchádzajúci prvok
        }
    }
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
int DLL_IsActive( DLList *list ) {
	return (list->activeElement != NULL) ? 1 : 0;	// vráti nenulovú hodnotu, ak je prvok aktívny, inak vráti nulu
}

/* Konec c206.c */
