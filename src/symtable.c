#include "symtable.h"

HashKey SymTable_entrykey(HashEntry entry) {
    return ((SymTableEntry *)entry)->id;
}

SymbolTable *SymTable_new(SymbolTable *parent) {
    SymbolTable *st = malloc(sizeof(SymbolTable));
    st->parent = parent;
    st->child = st->sibling = NULL;
    st->table = *Hash_new(sizeof(SymTableEntry), SymTable_entrykey, 20);
    return st;
}
