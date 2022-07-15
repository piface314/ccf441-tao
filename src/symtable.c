#include "symtable.h"

HashKey SymTable_entrykey(HashEntry entry) {
    SymTableEntry *e = (SymTableEntry *)entry;
    return e->id ? e->id->id : NULL;
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

void SymEntry_show_scope(ScopeNode *p) {
    if (p == NULL)
        return;
    SymEntry_show_scope(p->scope);
    printf("%s.", p->id);
}

void SymEntry_show_id(IdNode *p) {
    if (p == NULL)
        return;
    SymEntry_show_scope(p->scope);
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
    TypeNode *p = (p_->tag == NT_YIN) ? ((YinNode *)p_)->type : (TypeNode *)p_;
    SymEntry_show_ptr(p->ptr_t);
    if (p->tag == NT_VAR_TYPE) {
        SymEntry_show_id(p->id);
        if (p->arity == 0)
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
    SymEntry_show_id(entry->id);
    printf(": ");
    SymEntry_show_type(entry->type);
    printf("\n");
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

SymTableEntry *SymTableEntry_new(IdNode *id, TypeNode *type) {
    SymTableEntry *entry = malloc(sizeof(SymTableEntry));
    entry->id = id;
    entry->type = type;
    return entry;
}