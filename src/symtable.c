#include "symtable.h"

HashKey SymTable_entrykey(HashEntry entry) {
    return ((SymTableEntry *)entry)->id;
}

SymbolTable *SymTable_new(SymbolTable *parent) {
    SymbolTable *st = malloc(sizeof(SymbolTable));
    st->parent = parent;
    st->child = st->sibling = NULL;
    st->table = *Hash_new(sizeof(SymTableEntry), SymTable_entrykey, 20);
    if (parent != NULL)
        SymTable_append(st, parent);
    return st;
}

void SymTable_append(SymbolTable *child, SymbolTable *parent) {
    if (parent->child == NULL) {
        parent->child = child;
        return;
    }
    SymbolTable *current = NULL;
    for (current = parent->child; current->sibling != NULL; current = current->sibling)
        ;
    current->sibling = child;
}

void SymEntry_show(SymTableEntry *entry) {
    printf("%s\n", entry->id);
}

void SymTable_show_r(int offset, SymbolTable *t) {
    if (t == NULL) {
        for (int i = 0; i < offset; ++i)
            printf("    ");
        printf("--//--\n");
        return;
    }
    for (int i = 0; i < offset; ++i)
        printf("    ");
    printf("+=======+\n");
    void **entries = Hash_entries(&t->table);
    for (int i = 0; entries[i] != NULL; ++i) {
        for (int i = 0; i < offset; ++i)
            printf("    ");
        printf("| ");
        SymEntry_show(entries[i]);
    }
    for (SymbolTable *current = t->child; current != NULL; current = current->sibling)
        SymTable_show_r(offset + 1, current);
}

void SymTable_show(SymbolTable *t) {
    SymTable_show_r(0, t);
}

void SymTable_install(SymTableEntry *entry, SymbolTable *t) {
    HashTable *ht = Hash_add(entry, &t->table);
    if (ht != &t->table)
        t->table = *ht;
}
