/** IFJ2024
 * xsidlil00; Lukáš Šidlík
 */

#ifndef SYMTABLE
#define SYMTABLE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "dymString.h"
#include "token_types.h"

#define SYMTABLE_SIZE 41  // velikost symbol table

/**
 * @enum data type
 */
typedef enum
{
    DATA_TYPE_INT,     // integer
    DATA_TYPE_DOUBLE,  // double
    DATA_TYPE_U8,      // unsigned char
    DATA_TYPE_STRING,  // string
    DATA_TYPE_BOOLEAN, // boolean
    DATA_TYPE_VOID,    // void
    DATA_TYPE_UND      // undefined
} data_type_t;

/**
 * @struct data
 */
typedef struct data_t
{
    data_type_t type;      // datový typ dat
    DymString *parameters; // parametry funkce
    char *id;              // identifikátor
    bool init;             // inicializování
    bool canNull;          // nullability
    bool isConst;          // konstanta
    bool used;             // použití
} data_t;

/**
 * @struct symtable polozka
 */
typedef struct symtable_item_t
{
    char *key;                    // klíč
    data_t data;                  // data
    struct symtable_item_t *next; // ukazatel na dalsí polozku
} symtable_item_t;

/**
 * @brief symbol table
 */
typedef symtable_item_t *symtable_t[SYMTABLE_SIZE];

/**
 * @brief Inicializuje symbol table.
 *
 * @param symtable symbol table, které chceme inicializovat
 *
 * @return true, pokud byla inicializace úspěšná, false, pokud
 *     k nějaké chybě došlo.
 */
bool symtable_init(symtable_t *symtable);

/**
 * @brief Najde a vráti polozku v symtable se zadanym jménem.
 *
 * @param symtable tabulka symbolů, ve které chceme hledat
 * @param name  jméno, které chceme najít
 * @return Najde polozku nebo NULL, pokud taková položka neexistuje.
 */
data_t *symtable_get_item(symtable_t *table, char *name);

/**
 * @brief Vloží nový item do symtable
 * @param symtable Tabulka symbolů
 * @param name Jméno které chceme přidat
 * @return Ukazatel na nový item v symtable, pokud nastala vnitřní chyba, tak NULL
 */
data_t *symtable_insert(symtable_t *symtable, char *name);

/**
 * @brief Přidá parametr do řetězce parametrů data_t.
 *
 * @param data data_t, do které chceme přidat parametr
 * @param type typ parametru, který chceme přidat 
 * @return true, pokud se podařilo přidat parametr, jinak false
 */
bool symtable_insert_params(data_t *data, token_type type);

/**
 * @brief Zruší symbol table a dealokuje všechna alokovaná místa.
 * @param symtable Symbol table k zrušeni.
 * @return true, pokud se podařilo symbol table zrušit, jinak false.
 */
bool symtable_destroy(symtable_t *symtable);

/**
 * @brief Vloží builtin funkce do symbol table.
 *
 * Funkce vloží všechny builtin funkce do symbol table a nastaví jejich parametry.
 *
 * @param symtable Symbol table, do které chceme vložit builtin funkce
 * @return true, pokud se podařilo vložit builtin funkce, jinak false
 */
bool symtable_insert_builtin(symtable_t *symtable);

#endif