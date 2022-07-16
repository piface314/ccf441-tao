#include "symtable.h"

HashKey SymTable_entrykey(HashEntry entry) {
    SymTableEntry *e = (SymTableEntry *)entry;
    ASTNode *node = e->node;
    if (node == NULL)
        return NULL;
    switch (node->tag & NTMASK) {
    case NT_DEF:
        return node->def_node.id;
    case NT_TYPE_DEF:
        return node->type_def_node.decl->id->id;
    case NT_TYPE_ALIAS:
        return node->type_alias_node.decl->id->id;
    default:
        return NULL;
    }
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

void SymEntry_show_id(IdNode *p) {
    if (p == NULL)
        return;
    printf("%s", p->id);
}

void SymEntry_show_ptr(PtrTypeNode *p) {
    if (p == NULL)
        return;
    SymEntry_show_ptr(p->ptr_t);
    if (p->size)
        printf("[%d]", p->size);
    else
        printf("@");
}

void SymEntry_show_type(ASTNode *p_) {
    if (p_ == NULL) {
        printf("_");
        return;
    }
    TypeNode *p = (p_->tag == NT_YIN) ? ((DefNode *)p_)->type : (TypeNode *)p_;
    SymEntry_show_ptr(p->ptr_t);
    if (p->tag == NT_VAR_TYPE) {
        SymEntry_show_id(p->id);
        if (p->arity == 0)
            return;
    } else if (p->tag & NT_ID) {
        SymEntry_show_id((IdNode *)p);
        return;
    }
    printf("(");
    for (int i = 0; i < p->arity; ++i) {
        if (i) printf(", ");
        SymEntry_show_type(p->params[i]);
    }
    printf(")");
    if (p->tag == NT_FUN_TYPE) {
        printf(" |||::| ");
        SymEntry_show_type(p->params[p->arity]);
    } else if (p->tag == NT_PROC_TYPE) {
        printf(" |||:||");
    }
}

void SymEntry_show(SymTableEntry *entry) {
    ASTNode *node = entry->node;
    Loc loc = node->id_node.loc;
    printf("[%3d,%3d] ", loc.line, loc.col);
    switch (node->tag & NTMASK) {
    case NT_DEF:
        printf("%8s: ", node->def_node.id);
        SymEntry_show_type((ASTNode *)node->def_node.type);
        break;
    case NT_TYPE_DEF:
        printf("%8s  ", "");
        SymEntry_show_type((ASTNode *)node->type_def_node.decl);
        break;
    case NT_TYPE_ALIAS:
        printf("%8s  ", "");
        SymEntry_show_type((ASTNode *)node->type_alias_node.decl);
        break;
    default: printf("?");
    }
    printf("\n");
}

void SymTable_show_r(int offset, SymbolTable *t) {
    if (t == NULL) {
        for (int i = 0; i < offset; ++i)
            printf("    ");
        printf("+================//=================+\n");
        return;
    }
    for (int i = 0; i < offset; ++i)
        printf("    ");
    printf("+===================================+\n");
    void **entries = Hash_entries(&t->table);
    for (int i = 0; entries[i] != NULL; ++i) {
        for (int i = 0; i < offset; ++i)
            printf("    ");
        printf("| ");
        SymEntry_show(entries[i]);
    }
    free(entries);
    for (int i = 0; i < offset; ++i)
        printf("    ");
    printf("+================//=================+\n");
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

SymTableEntry *SymTableEntry_new(ASTNode *node) {
    SymTableEntry *entry = malloc(sizeof(SymTableEntry));
    entry->node = node;
    return entry;
}

SymTableEntry *SymTable_lookup(char *id, SymbolTable *t) {
    SymTableEntry *entry = NULL;
    for (; t; t = t->parent) {
        entry = Hash_get(id, &t->table);
        if (entry != NULL)
            break;
    }
    return entry;
}