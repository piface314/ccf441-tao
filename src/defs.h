#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
#include <string>
#include <vector>

class Loc {
    private:
    public:
    int line, col;
};

#define NTMASK 0xfffffff0

enum NodeTag {
    NT_TYPE = 0x10,
    NT_VAR_TYPE = 0x11,
    NT_FUN_TYPE = 0x12,
    NT_PROC_TYPE = 0x13,
    NT_DEF = 0x20,
    NT_YIN = 0x21,
    NT_YANG = 0x22,
    NT_WUJI = 0x23,
    NT_CONSTRUCTOR = 0x24,
    NT_LITERAL = 0x30,
    NT_INT_LIT = 0x31,
    NT_REAL_LIT = 0x32,
    NT_CHAR_LIT = 0x33,
    NT_STR_LIT = 0x34,
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
    NT_CALL_TYPE_ALIAS = 0x91,
    NT_DECONSTRUCT = 0xA0
};

typedef union ast_node ASTNode;

class IdNode {
    public:
    NodeTag tag;
    Loc loc;
    char* id;
};

ASTNode *Node_pro_id(Loc loc, char* id);
ASTNode *Node_com_id(Loc loc, char* id);
ASTNode *Node_sym_id(Loc loc, char* id);

class PtrTypeNode {
    public:
    NodeTag tag;
    Loc loc;
    class PtrTypeNode* ptr_t;
    int size;
};

ASTNode *Node_ptr_type(Loc loc, ASTNode *ptr, int size);

class TypeNode {
    public:
    NodeTag tag;
    Loc loc;
    PtrTypeNode *ptr_t;
    IdNode *id;
    int arity;
    ASTNode **params;
};

ASTNode *Node_var_type(Loc loc, ASTNode *ptr, ASTNode *id, std::vector<const void*>& params);
ASTNode *Node_fun_type(Loc loc, ASTNode *ptr, std::vector<const void*>& params);
ASTNode *Node_proc_type(Loc loc, ASTNode *ptr, std::vector<const void*>& params);
ASTNode *Node_type_decl(Loc loc, char* id, std::vector<const void*>& params);

class DefNode {
    public:
    NodeTag tag;
    Loc loc;
    char* id;
    TypeNode *type;
    ASTNode *body;
};

ASTNode *Node_yin(Loc loc, char* id, ASTNode *type, ASTNode *init);
ASTNode *Node_yang(Loc loc, char* id, ASTNode *type, ASTNode *expr);
ASTNode *Node_wuji(Loc loc, char* id, ASTNode *type, ASTNode *body);
ASTNode *Node_constructor(Loc loc, char* id, std::vector<const void*>& params);
ASTNode *Node_adj_constr(ASTNode *type, ASTNode *node);

class ImportNode {
    public:
    NodeTag tag;
    Loc loc;
    IdNode *source;
    char* id;
};

class TypeDefNode {
    public:
    NodeTag tag;
    Loc loc;
    TypeNode *decl;
    int n_cons;
    DefNode **cons;
};

ASTNode *Node_type_def(Loc loc, ASTNode *decl);
ASTNode *Node_adj_type_def(std::vector<const void*>& cons, ASTNode *node);

class TypeAliasNode {
    public:
    NodeTag tag;
    Loc loc;
    TypeNode *decl;
    TypeNode *type;
};

ASTNode *Node_type_alias(Loc loc, ASTNode *decl, ASTNode *type);
ASTNode *Node_call_type_alias(Loc loc, char* id);

class LiteralNode {
    public:
    NodeTag tag;
    Loc loc;
    union {
        int int_v;
        double real_v;
        char char_v;
        char* str_v;
    } val;
};

class DeconsNode {
    public:
    NodeTag tag;
    Loc loc;
    IdNode *id;
    int argc;
    DefNode **args;
};

ASTNode *Node_decons(Loc loc, ASTNode *id, std::vector<const void*>& args, ASTNode *constr);

union ast_node {
    NodeTag tag;
    DefNode def_node;
    IdNode id_node;
    TypeNode type_node;
    PtrTypeNode ptr_type_node;
    ImportNode import_node;
    TypeDefNode type_def_node;
    TypeAliasNode type_alias_node;
    DeconsNode decons_node;
};

#endif