#include "hash.h"

HashEntry Hash_at(size_t index, HashTable *table) {
    return table->entries + index * table->entry_size;
}

HashTable *Hash_new(size_t entry_size, HashKey (*key)(HashEntry), size_t size) {
    HashTable *table = malloc(sizeof(HashTable));
    if (!size)
        size = 20;
    table->entry_size = entry_size;
    table->size = size;
    table->key = key;
    table->entries = malloc(entry_size * size);
    Hash_weights(table);
    for (size_t i = 0; i < size; ++i) {
        HashKey ekey = key(Hash_at(i, table));
        if (ekey)
            ekey[0] = 0;
    }
    return table;
}

HashTable *Hash_rehash(HashTable *t) {
    HashTable *retable = Hash_new(t->entry_size, t->key, t->size << 1);
    for (size_t i = 0; i < t->size; ++i)
        retable = Hash_add(Hash_at(i, t), retable);
    return retable;
}

void Hash_weights(HashTable *table) {
    for (size_t i = 0; i < KEYMAX; i++)
        table->w[i] = 1 + (size_t)((10000.0 * rand())/(RAND_MAX + 1.0));
}

size_t Hash_k(HashKey key, HashTable *table) {
    size_t sum = 0;
    for (size_t i = 0; key[i]; i++)
        sum += key[i] * table->w[i];
    return sum;
}

size_t Hash_h1(HashKey key, HashTable *table) {
    return Hash_k(key, table) % table->size;
}

size_t Hash_h2(HashKey key, HashTable *table) {
    return 1 + (Hash_k(key, table) % (table->size - 1));
}

HashTable *Hash_add(HashEntry entry, HashTable *table) {
    size_t i = 0;
    size_t h = Hash_h1(table->key(entry), table);
    size_t h2 = Hash_h2(table->key(entry), table);
    HashKey k = NULL;

    while ((k = table->key(Hash_at((h + i * h2) % table->size, table))) && k[0] && i < table->size)
        i++;

    if (i >= table->size)
        return Hash_add(entry, Hash_rehash(table));

    memcpy(Hash_at((h + i * h2) % table->size, table), entry, table->entry_size);
    return table;
}

HashEntry Hash_get(HashKey key, HashTable *table) {
    size_t i = 0;
    size_t h = Hash_h1(key, table);
    size_t h2 = Hash_h2(key, table);
    HashKey k = NULL;
    
    while ((k = table->key(Hash_at((h + i * h2) % table->size, table))) && strcmp(k, key) != 0 && i < table->size)
        i++;

    HashEntry entry = Hash_at((h + i * h2) % table->size, table);
    return (k = table->key(entry)) && strcmp(k, key) == 0 ? entry : NULL;
}

void Hash_show(HashTable *table) {
    printf("=========\n");
    for (size_t i = 0; i < table->size; ++i) {
        HashKey key = table->key(Hash_at(i, table));
        if (key && key[0])
            printf("[%lu]: %s\n", i, key);
    }
    printf("=========\n");
}

size_t Hash_size(HashTable *table) {
    size_t n = 0;
    for (size_t i = 0; i < table->size; ++i) {
        HashKey key = table->key(Hash_at(i, table));
        if (key && key[0])
            n++;
    }
    return n;
}

void **Hash_entries(HashTable *table) {
    size_t j = 0;
    void **entries = malloc(sizeof(void *) * table->size);
    for (size_t i = 0; i < table->size; ++i) {
        void *entry = Hash_at(i, table);
        HashKey key = table->key(entry);
        if (key && key[0])
            entries[j++] = entry;
    }
    entries[j] = NULL;
    return entries;
}
