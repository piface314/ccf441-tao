#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEYMAX 128

typedef void* HashEntry;
typedef char* HashKey;
typedef struct {
    size_t entry_size;
    size_t w[KEYMAX];
    size_t size;
    void *entries;
    HashKey (*key)(void *);
} HashTable;

HashEntry Hash_at(size_t index, HashTable *table);
HashTable *Hash_new(size_t entry_size, HashKey (*key)(HashEntry), size_t size);
HashTable *Hash_rehash(HashTable *table);
void Hash_weights(HashTable *table);
size_t Hash_k(HashKey key, HashTable *table);
size_t Hash_h1(HashKey key, HashTable *table);
size_t Hash_h2(HashKey key, HashTable *table);
HashTable *Hash_add(HashEntry entry, HashTable *table);
HashEntry Hash_get(HashKey key, HashTable *table);
void Hash_show(HashTable *table);
size_t Hash_size(HashTable *table);
void **Hash_entries(HashTable *table);

#endif