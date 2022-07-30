#include "symtable.hpp"

Loc::Loc() {
    this->line = this->col = 0;
}

Loc::Loc(YYLTYPE l) {
    this->line = l.first_line;
    this->col = l.first_column;
}

SymTableEntry::SymTableEntry(Loc loc, ASTNode *node) {
    this->loc = loc;
    this->node = node;
}

SymTable::SymTable() {
    this->prev = NULL;

    // Tipos básicos
    std::vector<ASTNode*> empty;
    std::vector<std::string> types = { "Char", "Int", "Real", "Any" };
    std::vector<std::string> numeric_types = { "Char", "Int", "Real" };
    std::vector<std::string> integer_types = { "Char", "Int" };
    for (auto it = types.begin(); it != types.end(); ++it)
        this->install(*it, SymTableEntry(Loc(), new TypeDefNode(*it, empty)));
    
    // Booleanos
    std::vector<ASTNode*> bools;
    std::vector<std::string> bool_ids = { "False", "True" };
    for (auto it = bool_ids.begin(); it != bool_ids.end(); ++it) {
        ASTNode *c = new ConstrNode(*it, empty);
        bools.push_back(c);
        this->install(*it, SymTableEntry(Loc(), c));
    }
    TypeDefNode *tdnode = new TypeDefNode("Bool", empty);
    tdnode->add_constrs(bools);
    this->install("Bool", SymTableEntry(Loc(), tdnode));

    // Null
    ConstrNode *nullc = new ConstrNode("Null", empty);
    nullc->ret = new PtrTypeNode(0, new VarTypeNode("Any", empty));
    this->install("Null", SymTableEntry(Loc(), nullc));
    
    // Operadores
    std::vector<std::string> numeric_ops = { "**", "*", "/", "+", "-" };
    std::vector<std::string> bitwise_ops = { "&", "^", "|", "<<", ">>" };
    std::vector<std::string> logical_ops = { "&&", "||" };
    std::vector<std::string> compare_ops = { "==", "!=", "<", "<=", ">=", ">" };

    for (auto it = numeric_ops.begin(); it != numeric_ops.end(); ++it) {
        for (auto it2 = numeric_types.begin(); it2 != numeric_types.end(); ++it2) {
            std::vector<ASTNode*> params = {
                new ParamNode("x", new VarTypeNode(*it2, empty)),
                new ParamNode("y", new VarTypeNode(*it2, empty)),
            };
            ASTNode *ret = new VarTypeNode(*it2, empty);
            this->install(*it, SymTableEntry(Loc(), new YangNode(*it, params, ret)));
        }
    }
    
    for (auto it = bitwise_ops.begin(); it != bitwise_ops.end(); ++it) {
        for (auto it2 = integer_types.begin(); it2 != integer_types.end(); ++it2) {
            std::vector<ASTNode*> params = {
                new ParamNode("a", new VarTypeNode(*it2, empty)),
                new ParamNode("b", new VarTypeNode(*it2, empty)),
            };
            ASTNode *ret = new VarTypeNode(*it2, empty);
            this->install(*it, SymTableEntry(Loc(), new YangNode(*it, params, ret)));
        }
    }

    for (auto it = logical_ops.begin(); it != logical_ops.end(); ++it) {
        std::vector<ASTNode*> params = {
            new ParamNode("a", new VarTypeNode("Boolean", empty)),
            new ParamNode("b", new VarTypeNode("Boolean", empty)),
        };
        ASTNode *ret = new VarTypeNode("Boolean", empty);
        this->install(*it, SymTableEntry(Loc(), new YangNode(*it, params, ret)));
    }

    for (auto it = compare_ops.begin(); it != compare_ops.end(); ++it) {
        for (auto it2 = types.begin(); it2 != types.end(); ++it2) {
            std::vector<ASTNode*> params = {
                new ParamNode("a", new VarTypeNode(*it2, empty)),
                new ParamNode("b", new VarTypeNode(*it2, empty)),
            };
            ASTNode *ret = new VarTypeNode(*it2, empty);
            this->install(*it, SymTableEntry(Loc(), new YangNode(*it, params, ret)));
        }
    }
}

SymTable::SymTable(SymTable *prev) {
    this->prev = prev;
}

std::ostream & operator << (std::ostream &out, const SymTable &c) {
    return out;
}

void SymTable::install(std::string key, const SymTableEntry &entry) {
    std::stack<SymTableEntry> &s = this->table[key];
    s.push(entry);
}


std::stack<SymTableEntry> *SymTable::lookup(std::string key) {
    SymTable *t = this;
    for (; t; t = t->prev) {
        auto it = t->table.find(key);
        if (it != t->table.end())
            return &it->second;
    }
    return NULL;
}