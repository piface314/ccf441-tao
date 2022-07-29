#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <iostream>
#include <string>
#include <map>
#include "ast.hpp"
#include "parser.hpp"

class Loc {
public:
    int line, col;
    Loc();
    Loc(YYLTYPE l);
};

class SymTableEntry {
public:
    Loc loc;
    ASTNode *node;
    SymTableEntry(Loc loc, ASTNode *node);
};

class SymTable {
public:
    SymTable *prev;
    std::map<std::string,SymTableEntry *> table;
    SymTable();
    SymTable(SymTable *prev);
    friend std::ostream & operator << (std::ostream &out, const SymTable &t);
    void install(std::string key, SymTableEntry *entry);
    SymTableEntry *lookup(std::string key);
};

#endif