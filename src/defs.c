#include "defs.h"
#include <stdio.h>
#include <string.h>

String Str_empty() {
    String s = {.n = 0, .s = malloc(sizeof(char))};
    return s;
}

String Str_concat(String a, String b) {
    size_t n = a.n + b.n;
    String s = { .n = n, .s = malloc(sizeof(char)*(n+1)) };
    strcpy(s.s, a.s);
    strcat(s.s, b.s);
    return s;
}

String Str_prefix(char *a, String b) {
    size_t n = strlen(a) + b.n;
    String s = { .n = n, .s = malloc(sizeof(char)*(n+1)) };
    strcpy(s.s, a);
    strcat(s.s, b.s);
    return s;
}

String Str_suffix(String a, char *b) {
    size_t n = a.n + strlen(b);
    String s = { .n = n, .s = malloc(sizeof(char)*(n+1)) };
    strcpy(s.s, a.s);
    strcat(s.s, b);
    return s;
}

ASTNode *Node_scope(ASTNode *scope, char *id) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->scope_node.tag = NT_SCOPE;
    node->scope_node.scope = scope;
    node->scope_node.id = id;
    return node;
}

ASTNode *Node_ptr_type(ASTNode *ptr, int size) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->ptr_type_node.tag = NT_PTR_TYPE;
    node->ptr_type_node.ptr_t = (PtrTypeNode *)ptr;
    node->ptr_type_node.size = size;
    return node;
}

ASTNode *Node_id(NodeTag tag, ASTNode *scope, char *id) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->id_node.tag = tag;
    node->id_node.scope = scope;
    node->id_node.id = id;
    return node;
}

ASTNode *Node_pro_id(ASTNode *scope, char *id) { return Node_id(NT_PROID, scope, id); }
ASTNode *Node_com_id(ASTNode *scope, char *id) { return Node_id(NT_COMID, scope, id); }
ASTNode *Node_sym_id(ASTNode *scope, char *id) { return Node_id(NT_SYMID, scope, id); }

ASTNode *Node_type(NodeTag tag, ASTNode *ptr, ASTNode *id, List *params) {
    int arity = List_size(params), i = 1;
    TypeNode **p = arity ? malloc(sizeof(TypeNode *) * arity) : NULL;
    for (List *c = params; c; c = c->tail, ++i)
        p[arity-i] = (TypeNode *)c->item;
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type_node.tag = tag;
    node->type_node.ptr_t = ptr;
    node->type_node.id = id;
    node->type_node.arity = arity;
    node->type_node.params = p;
    return node;
}

ASTNode *Node_var_type(ASTNode *ptr, ASTNode *id, List *params) {
    return Node_type(NT_VAR_TYPE, ptr, id, params);
}

ASTNode *Node_fun_type(ASTNode *ptr, ASTNode *id, List *params) {
    ASTNode *node = Node_type(NT_FUN_TYPE, ptr, id, params);
    node->type_node.arity--;
    return node;
}

ASTNode *Node_proc_type(ASTNode *ptr, ASTNode *id, List *params) {
    return Node_type(NT_PROC_TYPE, ptr, id, params);
}
