#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "hash.h"
#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>


class SymTableEntry {
    public:
    HashKey key;
    ASTNode *node;
};

class SymbolTable {
    public:
    class SymbolTable *parent, *child, *sibling;
    HashTable table;
};

HashKey SymTable_entrykey(HashEntry entry);
SymbolTable *SymTable_new(SymbolTable *parent);
void SymTable_append(SymbolTable *child, SymbolTable *parent);
void SymTable_show(SymbolTable *t);
void SymTable_install(SymTableEntry *entry, SymbolTable *t);
SymTableEntry *SymTableEntry_new(ASTNode *node);
SymTableEntry *SymTable_lookup(char *id, SymbolTable *t);

#endif