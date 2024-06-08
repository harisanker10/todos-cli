#include "hashmap.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 16;

Table *ht_create() {
  Table *table = (Table *)malloc(sizeof(Table));
  if (table == NULL) {
    return NULL;
  }
  table->length = 0;
  table->capacity = INITIAL_CAPACITY;
  table->entries = (Entry *)calloc(table->capacity, sizeof(Entry));
  if (table->entries == NULL) {
    free(table);
    return NULL;
  }
  return table;
};

int destroyTable(Table *table) {
  if (table == NULL) {
    return -1;
  }
  for (int i = 0; i < table->capacity; i++) {
    free((char *)table->entries[i].key);
    free((char *)table->entries[i].val);
  }
  free(table->entries);
  free(table);
  return 0;
}

int ht_set(Table *table, char *key, void *value) {
  setEntry(table->entries, table->capacity, key, value);
  if (table->capacity / 2 < table->length) {
    resize(table);
  }
  table->length++;
  return 0;
}
void *ht_getAll(Table *table) {
  if (table->length < 1) {
    return NULL;
  }
  Entry *entries = malloc(sizeof(Entry) * table->length);
  if (entries == NULL) {
    return NULL;
  }
  int ptr = 0;
  for (int i = 0; i < table->capacity; i++) {
    if (table->entries[i].key != NULL) {
      entries[ptr] = table->entries[i];
      ptr++;
    }
  }
  return entries;
}

const void *ht_get(Table *table, char *key) {
  uint64_t hashKey = hash(key);
  int orgIndex = hashKey % table->capacity;
  int index = orgIndex;
  while (strcmp(table->entries[index].key, key) != 0 &&
         index < table->capacity) {
    index++;
    if (index == table->capacity) {
      index = 0;
    }
    if (index == orgIndex) {
      return NULL;
    }
  }
  const void *value = table->entries[index].val;
  return value;
}
