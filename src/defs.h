#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
#include "list.h"

typedef struct {
    char *s;
    size_t n;
} String;

String Str_empty();
String Str_concat(String a, String b);
String Str_prefix(char *a, String b);
String Str_suffix(String a, char *b);

typedef enum {
    NT_TYPE = 0x10,
    NT_VAR_TYPE = 0x11,
    NT_FUN_TYPE = 0x12,
    NT_PROC_TYPE = 0x13,
    NT_YIN = 0x20,
    NT_YANG = 0x30,
    NT_WUJI = 0x40,
    NT_SCOPE = 0x50,
    NT_ID = 0x60,
    NT_COMID = 0x61,
    NT_PROID = 0x62,
    NT_SYMID = 0x63,
    NT_PTR_TYPE = 0x70
} NodeTag;

typedef union ast_node ASTNode;

typedef struct scope_node {
    NodeTag tag;
    struct scope_node *scope;
    char *id;
} ScopeNode;

ASTNode *Node_scope(ASTNode *scope, char *id);

typedef struct {
    NodeTag tag;
    ScopeNode *scope;
    char *id;
} IdNode;

ASTNode *Node_pro_id(ASTNode *scope, char *id);
ASTNode *Node_com_id(ASTNode *scope, char *id);
ASTNode *Node_sym_id(ASTNode *scope, char *id);

typedef struct ptr_type_node {
    NodeTag tag;
    struct ptr_type_node *ptr_t;
    int size;
} PtrTypeNode;

ASTNode *Node_ptr_type(ASTNode *ptr, int size);

typedef struct type_node {
    NodeTag tag;
    PtrTypeNode *ptr_t;
    IdNode *id;
    int arity;
    ASTNode **params;
} TypeNode;

ASTNode *Node_var_type(ASTNode *ptr, ASTNode *id, List *params);
ASTNode *Node_fun_type(ASTNode *ptr, ASTNode *id, List *params);
ASTNode *Node_proc_type(ASTNode *ptr, ASTNode *id, List *params);

typedef struct {
    NodeTag tag;
    char *id;
    TypeNode *type;
} YinNode;

ASTNode *Node_yin(char *id, ASTNode *type);

typedef struct {
    NodeTag tag;
    char *id;
    TypeNode *type;
    ASTNode *expr;
} YangNode;

typedef struct {
    NodeTag tag;
    char *id;
    TypeNode *type;
    ASTNode *block;
} WujiNode;

typedef union ast_node {
    NodeTag tag;
    YinNode yin_node;
    YangNode yang_node;
    WujiNode wuji_node;
    ScopeNode scope_node;
    IdNode id_node;
    TypeNode type_node;
    PtrTypeNode ptr_type_node;
} ASTNode;

#endif