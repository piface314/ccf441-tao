#include "defs.h"
#include <stdio.h>
#include <string.h>

ASTNode *Node_ptr_type(Loc loc, ASTNode *ptr, int size) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->ptr_type_node.tag = NT_PTR_TYPE;
    node->ptr_type_node.loc = loc;
    node->ptr_type_node.ptr_t = (PtrTypeNode *)ptr;
    node->ptr_type_node.size = size;
    return node;
}

ASTNode *Node_id(NodeTag tag, Loc loc, char *id) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->id_node.tag = tag;
    node->id_node.loc = loc;
    node->id_node.id = id;
    return node;
}

ASTNode *Node_pro_id(Loc loc, char *id) { return Node_id(NT_PROID, loc, id); }
ASTNode *Node_com_id(Loc loc, char *id) { return Node_id(NT_COMID, loc, id); }
ASTNode *Node_sym_id(Loc loc, char *id) { return Node_id(NT_SYMID, loc, id); }

ASTNode *Node_type(NodeTag tag, Loc loc, ASTNode *ptr, ASTNode *id, List *params) {
    int arity = List_size(params), i = 1;
    ASTNode **p = arity ? malloc(sizeof(ASTNode *) * arity) : NULL;
    for (List *c = params; c; c = c->tail, ++i)
        p[arity-i] = (ASTNode *)c->item;
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type_node.tag = tag;
    node->type_node.loc = loc;
    node->type_node.ptr_t = (PtrTypeNode *)ptr;
    node->type_node.id = (IdNode *)id;
    node->type_node.arity = arity;
    node->type_node.params = p;
    return node;
}

ASTNode *Node_var_type(Loc loc, ASTNode *ptr, ASTNode *id, List *params) {
    return Node_type(NT_VAR_TYPE, loc, ptr, id, params);
}

ASTNode *Node_fun_type(Loc loc, ASTNode *ptr, List *params) {
    ASTNode *node = Node_type(NT_FUN_TYPE, loc, ptr, NULL, params);
    node->type_node.arity--;
    return node;
}

ASTNode *Node_proc_type(Loc loc, ASTNode *ptr, List *params) {
    return Node_type(NT_PROC_TYPE, loc, ptr, NULL, params);
}

ASTNode *Node_type_decl(Loc loc, char *id, List *params) {
    return Node_type(NT_VAR_TYPE, loc, NULL, Node_pro_id(loc, id), params);
}

ASTNode *Node_def(NodeTag tag, Loc loc, char *id, ASTNode *type, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->def_node.tag = tag;
    node->def_node.loc = loc;
    node->def_node.id = id;
    node->def_node.type = (TypeNode *)type;
    node->def_node.body = body;
    return node;
}

ASTNode *Node_yin(Loc loc, char *id, ASTNode *type, ASTNode *init) {
    return Node_def(NT_YIN, loc, id, type, init);
}

ASTNode *Node_yang(Loc loc, char *id, ASTNode *type, ASTNode *expr) {
    return Node_def(NT_YANG, loc, id, type, expr);
}

ASTNode *Node_wuji(Loc loc, char *id, ASTNode *type, ASTNode *body) {
    return Node_def(NT_WUJI, loc, id, type, body);
}

ASTNode *Node_constructor(Loc loc, char *id, List *params) {
    ASTNode *type_ = Node_fun_type(loc, NULL, List_push(NULL, params));
    return Node_def(NT_CONSTRUCTOR, loc, id, type_, NULL);
}

ASTNode *Node_adj_constr(ASTNode *type, ASTNode *node) {
    int i = node->def_node.type->arity;
    node->def_node.type->params[i] = type;
    return node;
}

ASTNode *Node_type_def(Loc loc, ASTNode *decl) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type_def_node.tag = NT_TYPE_DEF;
    node->type_def_node.loc = loc;
    node->type_def_node.decl = (TypeNode *)decl;
    return node;
}

ASTNode *Node_adj_type_def(List *cons, ASTNode *node) {
    int n_cons = List_size(cons), i = 1;
    DefNode **p = n_cons ? malloc(sizeof(DefNode *) * n_cons) : NULL;
    for (List *c = cons; c; c = c->tail, ++i)
        p[n_cons-i] = (DefNode *)c->item;
    node->type_def_node.n_cons = n_cons;
    node->type_def_node.cons = p;
    return node;
}

ASTNode *Node_type_alias(Loc loc, ASTNode *decl, ASTNode *type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type_alias_node.tag = NT_TYPE_ALIAS;
    node->type_alias_node.loc = loc;
    node->type_alias_node.decl = (TypeNode *)decl;
    node->type_alias_node.type = (TypeNode *)type;
    return node;
}

ASTNode *Node_def_type_alias(Loc loc, char *id) {
    ASTNode *decl = Node_type_decl(loc, id, NULL);
    ASTNode *node = Node_type_alias(loc, decl, NULL);
    node->tag = NT_DEF_TYPE_ALIAS;
    return node;
}
