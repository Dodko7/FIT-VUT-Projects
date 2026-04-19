#include "hashtable.h"
#include <stdlib.h>
#include <stdio.h>
/**
 * @file memory_table.h
 * @brief Modul pro dynamickou alokaci a ukladani pameti a jeji bezpecne uvolneni
 * @author Jakub Ramaseuski, xramas01
 * @date 2024
 */
#ifndef MEMORY_TABLE_H
#define MEMORY_TABLE_H

#define MEMORY_TABLE_SIZE 9973
size_t memt_hash(void *src);
void *memt_key(void *src);
void *memt_copy(void *src);
void memt_deinit(void **src);
bool memt_comp(void *src, void *other);
void memt_deinit(void **src);
void *imalloc(size_t size);
void *irealloc(void *ptr, size_t size);
void ifree(void *ptr);
_HASHTABLE(MEMORY_TABLE_SIZE, memory, void *, void *)

memory_ht _memory_table;
bool safe_memory = false; 
#endif