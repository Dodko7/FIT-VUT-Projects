/**
 * dynamický string pro parametry funkcí
 * Vytvořil: Lukáš Šidlík (xsidlil00)
 */

#ifndef DYMSTRING_H
#define DYMSTRING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_STRING_LENGTH 4 

/**
 * @struct Dynamický string pro parametry funkcí
 */
typedef struct DymString{
    char *data;         // data dynamického řetězce
    int max_length;     // maximální déllka dynamického řetězce
    int length;         // délka dynamického řetězce
}DymString;

/**
 * Inicializace dynamického řetězce.
 *
 * @param string Ukazatel na strukturu dynamického řetězce, která má být inicializována.
 *
 * @return Pokud je inicializace úspěšná, vrací true, jinak false.
 */
bool DymString_Init(DymString *string);

/**
 * Vloží znak insert na konec řetězce. Pokud délka řetězce dosáhne maximální délky, pak se délka řetězce zdvojnásobí.
 * 
 * @param string Ukazatel na strukturu dynamického řetězce.
 * @param insert Znak, který má být vložen na konec řetězce.
 * 
 * @return Pokud je vložení znaku úspěšné, vrací true, jinak false.
 */
bool DymString_Insert_Char(DymString *string, char insert);

/**
 * Uvolní paměť alokovanou pro dynamický řetězec.
 *
 * @param string Ukazatel na strukturu dynamického řetězce, jehož paměť má být uvolněna.
 */
void DymString_Destroy(DymString *string);

#endif