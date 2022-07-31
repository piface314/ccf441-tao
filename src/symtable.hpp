#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <iostream>
#include <string>
#include <vector>
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
    std::map<std::string,std::vector<SymTableEntry>> table;
    SymTable();
    SymTable(SymTable *prev);
    friend std::ostream & operator << (std::ostream &out, const SymTable &t);
    void install(std::string key, const SymTableEntry &entry);
    std::vector<SymTableEntry> *lookup(std::string key);
    std::vector<SymTableEntry> *lookup_all(std::string key);
};

#endif