#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "defs.h"

typedef struct {
    ASTNode *node;
} SymTableEntry;

typedef struct symbol_table {
    struct symbol_table *parent, *child, *sibling;
    HashTable table;
} SymbolTable;

HashKey SymTable_entrykey(HashEntry entry);
SymbolTable *SymTable_new(SymbolTable *parent);
void SymTable_append(SymbolTable *child, SymbolTable *parent);
void SymTable_show(SymbolTable *t);
void SymTable_install(SymTableEntry *entry, SymbolTable *t);
SymTableEntry *SymTableEntry_new(ASTNode *node);

#endif