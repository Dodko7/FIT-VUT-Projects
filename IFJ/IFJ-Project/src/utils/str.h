#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#ifndef STR_H
#define STR_H

// Struktura pro řetězec
typedef struct {
    char* data;      // Pointer na data
    size_t length;   // Aktuální délka řetězce
    size_t capacity; // Kapacita alokované paměti
} str_t;

void str_init(str_t* string);
void str_append(str_t* string, const char* source, ...);
void str_unify(str_t* string, str_t* tmp);
bool str_search(str_t* string, const char* substring);
void str_printout(str_t* string);
void str_clear(str_t* string);
void str_destroy(str_t* string);

#endif