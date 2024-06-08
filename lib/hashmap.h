#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *key;
  const void *val;
} Entry;

typedef struct Table {
  Entry *entries;
  size_t capacity;
  size_t length;
} Table;

static int resize(Table *table);
static uint64_t hash(const char *key);
const void *ht_get(Table *table, char *key);
static int setEntry(Entry *entries, size_t capacity, const char *key,
                    const void *value);
void *ht_getAll(Table *table);

Table *ht_create();
int ht_set(Table *table, char *key, void *value);
const void *ht_get(Table *table, char *key);
int destroyTable(Table *table);

static int setEntry(Entry *entries, size_t capacity, const char *key,
                    const void *value) {
  int orgindex = hash(key) % capacity;
  int index = orgindex;
  while (entries[index].key != NULL && index < capacity) {
    index++;
    if (index == capacity) {
      index = 0;
    }
    if (index == orgindex) {
      return -1;
    }
  }
  entries[index].key = strdup(key);
  entries[index].val = value;
  return 0;
}

static int resize(Table *table) {
  size_t newCapacity = table->capacity * 2;
  Entry *oldEntry = table->entries;
  Entry *newEntries = (Entry *)malloc(newCapacity * sizeof(Entry));
  for (size_t i = 0; i < table->capacity; i++) {
    if (oldEntry[i].key != NULL) {
      setEntry(newEntries, newCapacity, oldEntry[i].key, oldEntry[i].val);
    }
  }
  table->entries = newEntries;
  table->capacity = newCapacity;
  return 0;
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static uint64_t hash(const char *key) {
  const char *p = key;
  uint64_t hash = FNV_OFFSET;
  while (*p) {
    p++;
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}

#endif /* HASHMAP_H */
