#include "table.h"
#include "code.h"
#include "hash.h"
#include "memory.h"
#include "text.h"
#include <stdlib.h>
#include <string.h>

typedef unsigned long HashType;

HashTable hash_table_create(size_t size) {
  HashTable table;
  size_t i;
  table.table = NULL;
  table.table = resize_memory(table.table, size * sizeof(HashTableEntry));
  for (i = 0; i < size; ++i) {
    table.table[i].key = NULL;
    table.table[i].value = NULL;
  }
  table.size = size;
  return table;
}

void hash_table_free(HashTable *table) {
  HashTableEntry *entry;
  size_t i;
  for (i = 0; i < table->size; ++i) {
    entry = table->table + i;
    entry->key = resize_memory(entry->key, 0);
    entry->value = resize_memory(entry->value, 0);
  }
  /* Set the entry pointer to null to avoid having an invalid pointer. */
  entry = NULL;
  table->table = resize_memory(table->table, 0);
  table->size = 0;
}

static size_t index_from_hash(HashTable *table, const HashType hash) {
  return hash % table->size;
}

static HashTableEntry *hash_table_get_by_hash(HashTable *table,
                                              const HashType hash) {
  return table->table + index_from_hash(table, hash);
}

HashTableEntry *hash_table_get_by_string(HashTable *table, const char *string) {
  return hash_table_get_by_hash(table, hash_string(string));
}

static void hash_table_set_entry(HashTableEntry *entry, const char *string,
                                 size_t value_size, void *value) {
  const size_t key_size = strlen(string) + 1;
  /* Make a copy of the key. */
  entry->key = resize_memory(entry->key, key_size);
  copy_string(entry->key, string, key_size);
  /* Make a copy of the value. */
  entry->value = resize_memory(entry->value, value_size);
  memcpy(entry->value, value, value_size);
}

Code hash_table_put_by_string(HashTable *table, const char *string,
                              size_t value_size, void *value) {
  const HashType hash = hash_string(string);
  const size_t starting_index = index_from_hash(table, hash);
  size_t index = starting_index;
  HashTableEntry *entry = NULL;
  int found = 0;
  while (!found) {
    entry = table->table + index;
    if (entry->key == NULL) {
      found = 1;
      break;
    }
    index = (index + 1) % table->size;
    /* Stop looking for a bucket after a full cycle. */
    if (index == starting_index) {
      break;
    }
  }
  if (!found) {
    return CODE_ERROR;
  }
  hash_table_set_entry(entry, string, value_size, value);
  return CODE_OK;
}
