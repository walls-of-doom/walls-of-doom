#ifndef TABLE_H
#define TABLE_H

#include "code.h"
#include <stdlib.h>

typedef struct HashTableEntry {
  void *key;
  void *value;
} HashTableEntry;

typedef struct HashTable {
  HashTableEntry *table;
  size_t size;
} HashTable;

HashTable hash_table_create(const size_t size);

void hash_table_free(HashTable *table);

HashTableEntry *hash_table_get_by_string(HashTable *table, const char *string);

Code hash_table_put_by_string(HashTable *table, const char *string,
                              size_t value_size, void *value);

#endif
