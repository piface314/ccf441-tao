#include "defs.h"
#include <vector> 
#include <stdio.h>
#include <string.h>

ASTNode *Node_ptr_type(Loc loc, ASTNode *ptr, int size) {
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));
    node->ptr_type_node.tag = NodeTag::NT_PTR_TYPE;
    node->ptr_type_node.loc = loc;
    node->ptr_type_node.ptr_t = (PtrTypeNode*) ptr;
    node->ptr_type_node.size = size;
    return node;
}

ASTNode *Node_id(NodeTag tag, Loc loc, char* id) {
    ASTNode *node = (ASTNode*)  malloc(sizeof(ASTNode));
    node->id_node.tag = tag;
    node->id_node.loc = loc;
    node->id_node.id = id;
    return node;
}

ASTNode *Node_pro_id(Loc loc, char* id) { return Node_id(NodeTag::NT_PROID, loc, id); }
ASTNode *Node_com_id(Loc loc, char* id) { return Node_id(NodeTag::NT_COMID, loc, id); }
ASTNode *Node_sym_id(Loc loc, char* id) { return Node_id(NodeTag::NT_SYMID, loc, id); }

ASTNode *Node_type(NodeTag tag, Loc loc, ASTNode *ptr, ASTNode *id, std::vector<const void*>& params) {
    int arity = params.size(), i = 1;
    ASTNode **p = arity ? (ASTNode**) malloc(sizeof(ASTNode *) * arity) : NULL;
    for (auto &c: params) {
        p[arity-i] = (ASTNode *)c;
        i++;
    }
    ASTNode *node = (ASTNode*)  malloc(sizeof(ASTNode));
    node->type_node.tag = tag;
    node->type_node.loc = loc;
    node->type_node.ptr_t = (PtrTypeNode *)ptr;
    node->type_node.id = (IdNode *)id;
    node->type_node.arity = arity;
    node->type_node.params = p;
    return node;
}

ASTNode *Node_var_type(Loc loc, ASTNode *ptr, ASTNode *id, std::vector<const void*>& params) {
    return Node_type(NodeTag::NT_VAR_TYPE, loc, ptr, id, params);
}

ASTNode *Node_fun_type(Loc loc, ASTNode *ptr, std::vector<const void*>& params) {
    ASTNode *node = Node_type(NodeTag::NT_FUN_TYPE, loc, ptr, NULL, params);
    node->type_node.arity--;
    return node;
}

ASTNode *Node_proc_type(Loc loc, ASTNode *ptr, std::vector<const void*>& params) {
    return Node_type(NodeTag::NT_PROC_TYPE, loc, ptr, NULL, params);
}

ASTNode *Node_type_decl(Loc loc, char* id, std::vector<const void*>& params) {
    return Node_type(NodeTag::NT_VAR_TYPE, loc, NULL, Node_pro_id(loc, id), params);
}

ASTNode *Node_def(NodeTag tag, Loc loc, char* id, ASTNode *type, ASTNode *body) {
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));
    node->def_node.tag = tag;
    node->def_node.loc = loc;
    node->def_node.id = id;
    node->def_node.type = (TypeNode *)type;
    node->def_node.body = body;
    return node;
}

ASTNode *Node_yin(Loc loc, char* id, ASTNode *type, ASTNode *init) {
    return Node_def(NodeTag::NT_YIN, loc, id, type, init);
}

ASTNode *Node_yang(Loc loc, char* id, ASTNode *type, ASTNode *expr) {
    return Node_def(NodeTag::NT_YANG, loc, id, type, expr);
}

ASTNode *Node_wuji(Loc loc, char* id, ASTNode *type, ASTNode *body) {
    return Node_def(NodeTag::NT_WUJI, loc, id, type, body);
}

ASTNode *Node_constructor(Loc loc, char* id, std::vector<const void*>& params) {
    params.push_back(NULL);
    ASTNode *type_ = Node_fun_type(loc, NULL, params);
    return Node_def(NodeTag::NT_CONSTRUCTOR, loc, id, type_, NULL);
}

ASTNode *Node_adj_constr(ASTNode *type, ASTNode *node) {
    int i = node->def_node.type->arity;
    node->def_node.type->params[i] = type;
    return node;
}

ASTNode *Node_type_def(Loc loc, ASTNode *decl) {
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type_def_node.tag = NodeTag::NT_TYPE_DEF;
    node->type_def_node.loc = loc;
    node->type_def_node.decl = (TypeNode *)decl;
    return node;
}

ASTNode *Node_adj_type_def(std::vector<const void*>& cons, ASTNode *node) {
    int n_cons = cons.size(), i = 1;
    DefNode **p = n_cons ? (DefNode**) malloc(sizeof(DefNode *) * n_cons) : NULL;
    for (auto &c: cons) {
        p[n_cons-i] = (DefNode *)c;
        i++;
    }
    node->type_def_node.n_cons = n_cons;
    node->type_def_node.cons = p;
    return node;
}

ASTNode *Node_type_alias(Loc loc, ASTNode *decl, ASTNode *type) {
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type_alias_node.tag = NodeTag::NT_TYPE_ALIAS;
    node->type_alias_node.loc = loc;
    node->type_alias_node.decl = (TypeNode *)decl;
    node->type_alias_node.type = (TypeNode *)type;
    return node;
}

ASTNode *Node_call_type_alias(Loc loc, char* id) {
    std::vector<const void*> params;
    ASTNode *decl = Node_type_decl(loc, id, params);
    ASTNode *node = Node_type_alias(loc, decl, NULL);
    node->tag = NodeTag::NT_CALL_TYPE_ALIAS;
    return node;
}

ASTNode *Node_decons(Loc loc, ASTNode *id, std::vector<const void*>& args, ASTNode *constr) {
    int argc = args.size(), i = 1;
    DefNode **p = argc ? (DefNode**) malloc(sizeof(DefNode *) * argc) : NULL;
    for (auto &c: args) {
        IdNode *id_ = &((ASTNode *)c)->id_node;
        ASTNode *type = NULL;
        if (constr != NULL && constr->tag == NodeTag::NT_CONSTRUCTOR
            && argc-i < constr->def_node.type->arity)
            type = constr->def_node.type->params[argc-i];
        p[argc-i] = &(Node_yin(id_->loc, id_->id, type, NULL)->def_node);

        i++;
    }
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));
    node->decons_node.tag = NodeTag::NT_DECONSTRUCT;
    node->decons_node.loc = loc;
    node->decons_node.id = (IdNode *)id;
    node->decons_node.argc = argc;
    node->decons_node.args = p;
    return node;
}