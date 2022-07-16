#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
#include "list.h"

typedef struct {
    char *s;
    size_t n;
} String;

typedef struct {
    int line, col;
} Loc;

#define NTMASK 0xfffffff0

typedef enum {
    NT_TYPE = 0x10,
    NT_VAR_TYPE = 0x11,
    NT_FUN_TYPE = 0x12,
    NT_PROC_TYPE = 0x13,
    NT_DEF = 0x20,
    NT_YIN = 0x21,
    NT_YANG = 0x22,
    NT_WUJI = 0x23,
    NT_CONSTRUCTOR = 0x24,
    NT_SCOPE = 0x30,
    NT_ID = 0x40,
    NT_COMID = 0x41,
    NT_PROID = 0x42,
    NT_SYMID = 0x43,
    NT_PTR_TYPE = 0x50,
    NT_IMPORT = 0x60,
    NT_IMPORT_FROM = 0x61,
    NT_IMPORT_AS = 0x62,
    NT_TYPE_DEF = 0x80,
    NT_TYPE_ALIAS = 0x90,
    NT_DEF_TYPE_ALIAS = 0x91,
    NT_LITERAL = 0xA0,
    NT_INT_LIT = 0xA1,
    NT_REAL_LIT = 0xA2,
    NT_CHAR_LIT = 0xA3,
    NT_STR_LIT = 0xA4
} NodeTag;

typedef union ast_node ASTNode;

typedef struct scope_node {
    NodeTag tag;
    Loc loc;
    struct scope_node *scope;
    char *id;
} ScopeNode;

ASTNode *Node_scope(Loc loc, ASTNode *scope, char *id);

typedef struct {
    NodeTag tag;
    Loc loc;
    ScopeNode *scope;
    char *id;
} IdNode;

ASTNode *Node_pro_id(Loc loc, ASTNode *scope, char *id);
ASTNode *Node_com_id(Loc loc, ASTNode *scope, char *id);
ASTNode *Node_sym_id(Loc loc, ASTNode *scope, char *id);

typedef struct ptr_type_node {
    NodeTag tag;
    Loc loc;
    struct ptr_type_node *ptr_t;
    int size;
} PtrTypeNode;

ASTNode *Node_ptr_type(Loc loc, ASTNode *ptr, int size);

typedef struct type_node {
    NodeTag tag;
    Loc loc;
    PtrTypeNode *ptr_t;
    IdNode *id;
    int arity;
    ASTNode **params;
} TypeNode;

ASTNode *Node_var_type(Loc loc, ASTNode *ptr, ASTNode *id, List *params);
ASTNode *Node_fun_type(Loc loc, ASTNode *ptr, List *params);
ASTNode *Node_proc_type(Loc loc, ASTNode *ptr, List *params);
ASTNode *Node_type_decl(Loc loc, char *id, List *params);

typedef struct {
    NodeTag tag;
    Loc loc;
    char *id;
    TypeNode *type;
    ASTNode *body;
} DefNode;

ASTNode *Node_yin(Loc loc, char *id, ASTNode *type, ASTNode *init);
ASTNode *Node_yang(Loc loc, char *id, ASTNode *type, ASTNode *expr);
ASTNode *Node_wuji(Loc loc, char *id, ASTNode *type, ASTNode *body);
ASTNode *Node_constructor(Loc loc, char *id, List *params);
ASTNode *Node_adj_constr(ASTNode *type, ASTNode *node);

typedef struct {
    NodeTag tag;
    Loc loc;
    IdNode *source;
    char *id;
} ImportNode;

typedef struct {
    NodeTag tag;
    Loc loc;
    TypeNode *decl;
    int n_cons;
    DefNode **cons;
} TypeDefNode;

ASTNode *Node_type_def(Loc loc, ASTNode *decl);
ASTNode *Node_adj_type_def(List *cons, ASTNode *node);

typedef struct {
    NodeTag tag;
    Loc loc;
    TypeNode *decl;
    TypeNode *type;
} TypeAliasNode;

ASTNode *Node_type_alias(Loc loc, ASTNode *decl, ASTNode *type);
ASTNode *Node_def_type_alias(Loc loc, char *id);

typedef struct {
    NodeTag tag;
    Loc loc;
    union {
        int int_v;
        double real_v;
        char char_v;
        char *str_v;
    } val;
} LiteralNode;

typedef union ast_node {
    NodeTag tag;
    DefNode def_node;
    ScopeNode scope_node;
    IdNode id_node;
    TypeNode type_node;
    PtrTypeNode ptr_type_node;
    ImportNode import_node;
    TypeDefNode type_def_node;
    TypeAliasNode type_alias_node;
} ASTNode;

#endif