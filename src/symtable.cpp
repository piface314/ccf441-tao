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
    /*
    env = new SymTable(NULL);
    char *types[] = { "Int", "Long", "Char", "Real", "Bool", "Any" };
    ASTNode *p = NULL;
    for (int i = 0; i < (sizeof(types)/sizeof(char *)); ++i) {
        p = Node_type_def(no_loc(), Node_type_decl(no_loc(), types[i], NULL));
        SymTable_install(SymTableEntry_new(p), root);
    }
    p = Node_constructor(no_loc(), "Null", NULL);
    ASTNode *t = Node_type_decl(no_loc(), "Any", NULL);
    t->type_node.ptr_t = (PtrTypeNode *)Node_ptr_type(no_loc(), NULL, 0);
    p = Node_adj_constr(t, p);
    SymTable_install(SymTableEntry_new(p), root);
    
    // TODO:
    // adicionar os operadores predefinidos
    // >>> vai ser omitido por hora, deixa pra análise semântica
    */
}

SymTable::SymTable(SymTable *prev) {
    this->prev = prev;
}

std::ostream & operator << (std::ostream &out, const SymTable &c) {
    return out;
}

// void SymEntry_show_id(IdNode *p) {
//     if (p == NULL)
//         return;
//     printf("%s", p->id);
// }

// void SymEntry_show_ptr(PtrTypeNode *p) {
//     if (p == NULL)
//         return;
//     SymEntry_show_ptr(p->ptr_t);
//     if (p->size)
//         printf("[%d]", p->size);
//     else
//         printf("@");
// }

// void SymEntry_show_type(ASTNode *p_) {
//     if (p_ == NULL) {
//         printf("_");
//         return;
//     }
//     TypeNode *p = (p_->tag == NodeTag::NT_YIN) ? ((DefNode *)p_)->type : (TypeNode *)p_;
//     SymEntry_show_ptr(p->ptr_t);
//     if (p->tag == NodeTag::NT_VAR_TYPE) {
//         SymEntry_show_id(p->id);
//         if (p->arity == 0)
//             return;
//     } else if (p->tag & NodeTag::NT_ID) {
//         SymEntry_show_id((IdNode *)p);
//         return;
//     }
//     printf("(");
//     for (int i = 0; i < p->arity; ++i) {
//         if (i) printf(", ");
//         SymEntry_show_type(p->params[i]);
//     }
//     printf(")");
//     if (p->tag == NodeTag::NT_FUN_TYPE) {
//         printf(" |||::| ");
//         SymEntry_show_type(p->params[p->arity]);
//     } else if (p->tag == NodeTag::NT_PROC_TYPE) {
//         printf(" |||:||");
//     }
// }

// void SymEntry_show(SymTableEntry *entry) {
//     ASTNode *node = entry->node;
//     Loc loc = node->id_node.loc;
//     printf("[%3d,%3d] ", loc.line, loc.col);

//     switch (node->tag & NTMASK) {
//     case NodeTag::NT_DEF:
//         printf("%8s: ", node->def_node.id);
//         SymEntry_show_type((ASTNode *)node->def_node.type);
//         break;
//     case NodeTag::NT_TYPE_DEF:
//         printf("%8s  ", "");
//         SymEntry_show_type((ASTNode *)node->type_def_node.decl);
//         break;
//     case NodeTag::NT_TYPE_ALIAS:
//         printf("%8s  ", "");
//         SymEntry_show_type((ASTNode *)node->type_alias_node.decl);
//         break;
//     default: printf("?");
//     }
//     printf("\n");
// }

// void SymTable_show_r(int offset, SymbolTable *t) {
//     if (t == NULL) {
//         for (int i = 0; i < offset; ++i)
//             printf("    ");
//         printf("+================//=================+\n");
//         return;
//     }
//     for (int i = 0; i < offset; ++i)
//         printf("    ");
//     printf("+===================================+\n");
//     void **entries = Hash_entries(&t->table);
//     for (int i = 0; entries[i] != NULL; ++i) {
//         for (int i = 0; i < offset; ++i)
//             printf("    ");
//         printf("| ");
//         SymEntry_show((SymTableEntry*) entries[i]);
//     }
//     free(entries);
//     for (int i = 0; i < offset; ++i)
//         printf("    ");
//     printf("+================//=================+\n");
//     for (SymbolTable *current = t->child; current != NULL; current = current->sibling)
//         SymTable_show_r(offset + 1, current);
// }

// void SymTable_show(SymbolTable *t) {
//     SymTable_show_r(0, t);
// }

void SymTable::install(std::string key, SymTableEntry *entry) {
    this->table[key] = entry;
}


SymTableEntry *SymTable::lookup(std::string key) {
    SymTable *t = this;
    for (; t; t = t->prev) {
        auto entry = t->table[key];
        if (entry)
            return entry;
    }
    return NULL;
}