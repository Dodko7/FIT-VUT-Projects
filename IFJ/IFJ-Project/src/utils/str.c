#include "str.h"

// Inicializace řetězce
void str_init(str_t* string_t) {
    string_t->length = 0;
    string_t->capacity = 16;  // Počáteční kapacita
    string_t->data = (char*)malloc(string_t->capacity);
    if (string_t->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    string_t->data[0] = '\0';
}

// Přidání formátovaného řetězce
void str_append(str_t* string, const char* source, ...) {
    va_list args;
    va_start(args, source);
    
    // Nejprve zjistíme potřebnou délku
    va_list args_copy;
    va_copy(args_copy, args);
    int needed_length = vsnprintf(NULL, 0, source, args_copy);
    va_end(args_copy);
    
    // Kontrola, zda je potřeba zvětšit buffer
    size_t new_length = string->length + needed_length;
    if (new_length >= string->capacity) {
        size_t new_capacity = string->capacity;
        while (new_capacity <= new_length) {
            new_capacity *= 2;
        }
        char* new_data = (char*)realloc(string->data, new_capacity);
        if (new_data == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            va_end(args);
            return;
        }
        string->data = new_data;
        string->capacity = new_capacity;
    }
    
    // Formátování do bufferu
    vsnprintf(string->data + string->length, string->capacity - string->length, source, args);
    string->length = new_length;
    
    va_end(args);
}

// Spojení dvou řetězců
void str_unify(str_t* string, str_t* tmp) {
    str_append(string, "%s", tmp->data);
}

bool str_search(str_t* string, const char* substring) {
    if (string == NULL || string->data == NULL || substring == NULL) {
        return false;
    }
    return (strstr(string->data, substring) != NULL);
}

// Výpis řetězce
void str_printout(str_t* string) {
    printf("%s", string->data);
}

// Vyčištění řetězce
void str_clear(str_t* string) {
    string->data[0] = '\0';
    string->length = 0;
}

// Uvolnění paměti
void str_destroy(str_t* string) {
    free(string->data);
    string->data = NULL;
    string->length = 0;
    string->capacity = 0;
}