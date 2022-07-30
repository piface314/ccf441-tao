#ifndef AST_H
#define AST_H

#define VCOPY(T,V) for(auto it=V.begin();it!=V.end();++it)this->V.push_back(dynamic_cast<T*>(*it))
#define VDEL(V) for(auto it=V.begin();it!=V.end();++it)delete *it;
#define COPY(T,V) this->V = dynamic_cast<T*>(V)
#define VSHOW(V) out << "["; \
                for(auto it=V.begin();it!=V.end();++it) {\
                    out << **it; \
                    if (it+1!=V.end()) out << ", ";\
                }\
                out << "]"

#include <iostream>
#include <vector>
#include <string>
#include "defs.hpp"
#include <llvm/IR/Value.h>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual bool declares_type() { return false; };
    virtual std::ostream& show(std::ostream &out);
    virtual llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

std::ostream& operator<<(std::ostream& os, ASTNode& node);

class TypeNode : public ASTNode {
public:
    virtual ~TypeNode() = default;
};
class StmtNode : public ASTNode {
public:
    virtual ~StmtNode() = default;
};
class ExprNode : public StmtNode {
public:
    virtual ~ExprNode() = default;
};

union LiteralVal {
public:
    int n;
    double x;
    char c;
    std::string s;
    ~LiteralVal() {};
    LiteralVal() {};
};

class LiteralNode : public ExprNode {
public:
    int type_tag;
    LiteralVal val;
    ~LiteralNode() {};
    LiteralNode(int n);
    LiteralNode(double x);
    LiteralNode(char c);
    LiteralNode(std::string s);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class IDNode : public ExprNode {
public:
    std::string id;
    ~IDNode() {};
    IDNode(std::string id);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class ExportNode : public ASTNode {
public:
    std::string id;
    std::vector<IDNode*> exports;
    ~ExportNode();
    ExportNode(std::string id, std::vector<ASTNode*> &exports);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class ProgramNode : public ASTNode {
public:
    ExportNode *module_decl;
    std::vector<StmtNode*> stmts;
    ~ProgramNode();
    ProgramNode(ASTNode *module_decl, std::vector<ASTNode*> &stmts);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class ImportNode : public StmtNode {
public:
    std::string id;
    std::vector<IDNode*> imports;
    std::string alias;
    ~ImportNode();
    ImportNode(std::string id, std::vector<ASTNode*> &imports, std::string alias);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class ParamNode : public ASTNode {
public:
    std::string id;
    TypeNode *type;
    ~ParamNode();
    ParamNode(std::string id, ASTNode *type);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class TypeParamNode : public TypeNode {
public:
    std::string id;
    ~TypeParamNode();
    TypeParamNode(std::string id);
    bool declares_type() { return true; };
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class PtrTypeNode : public TypeNode {
public:
    int size;
    TypeNode *type;
    ~PtrTypeNode();
    PtrTypeNode(int size);
    PtrTypeNode(int size, TypeNode *type);
    void set_type(ASTNode *type);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class VarTypeNode : public TypeNode {
public:
    std::string id;
    std::vector<TypeNode *> params;
    ~VarTypeNode();
    VarTypeNode(std::string id, std::vector<ASTNode *> &params);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class FunTypeNode : public TypeNode {
public:
    std::vector<TypeNode *> params;
    TypeNode *ret;
    ~FunTypeNode();
    FunTypeNode(std::vector<ASTNode *> &params, ASTNode *ret);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class ProcTypeNode : public TypeNode {
public:
    std::vector<TypeNode *> params;
    ~ProcTypeNode();
    ProcTypeNode(std::vector<ASTNode *> &params);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class YinNode : public StmtNode {
public:
    std::string id;
    TypeNode *type;
    ExprNode *init;
    ~YinNode();
    YinNode(std::string id, ASTNode *type, ASTNode *init);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class CallableNode : public StmtNode {
public:
    std::string id;
    std::vector<ParamNode*> params;
    virtual ~CallableNode() = default;
    virtual void set_type_params(std::vector<ASTNode*> &t_params) {};
    virtual void set_body(ASTNode *body) {};
};

class YangNode : public CallableNode {
public:
    std::vector<TypeParamNode*> t_params;
    TypeNode *ret;
    ExprNode *body;
    ~YangNode();
    YangNode(std::string id, std::vector<ASTNode*> &params, ASTNode *ret);
    void set_type_params(std::vector<ASTNode*> &t_params);
    void set_body(ASTNode *body);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class WujiNode : public CallableNode {
public:
    std::vector<TypeParamNode*> t_params;
    StmtNode *body;
    ~WujiNode();
    WujiNode(std::string id, std::vector<ASTNode*> &params);
    void set_type_params(std::vector<ASTNode*> &t_params);
    void set_body(ASTNode *body);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class ConstrNode : public CallableNode {
public:
    TypeNode *ret;
    ~ConstrNode();
    ConstrNode(std::string id, std::vector<ASTNode *> &params);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class TypeDefNode : public StmtNode {
public:
    std::string id;
    std::vector<TypeParamNode*> params;
    std::vector<ConstrNode*> constrs;
    ~TypeDefNode();
    TypeDefNode(std::string id, std::vector<ASTNode*> &params);
    void add_constrs(std::vector<ASTNode*> &constrs);
    TypeNode *get_type();
    bool declares_type() { return true; };
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class TypeAliasNode : public StmtNode {
public:
    std::string id;
    std::vector<TypeParamNode*> params;
    TypeNode *type;
    ~TypeAliasNode();
    TypeAliasNode(std::string id, std::vector<ASTNode*> &params, ASTNode *type);
    bool declares_type() { return true; };
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class WhileNode : public StmtNode {
public:
    ExprNode *cond;
    StmtNode *step;
    StmtNode *body;
    ~WhileNode();
    WhileNode(ASTNode *cond, ASTNode *step, ASTNode *body);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class RepeatNode : public StmtNode {
public:
    StmtNode *body;
    ExprNode *cond;
    StmtNode *step;
    ~RepeatNode();
    RepeatNode(ASTNode *body, ASTNode *cond, ASTNode *step);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class BreakNode : public StmtNode {
public:
    ~BreakNode() {};
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class ContinueNode : public StmtNode {
public:
    ~ContinueNode() {};
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class ReturnNode : public StmtNode {
public:
    ExprNode *expr;
    ~ReturnNode();
    ReturnNode(ASTNode *expr);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class FreeNode : public StmtNode {
public:
    ExprNode *expr;
    ~FreeNode();
    FreeNode(ASTNode *expr);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class UnaryOpNode : public ExprNode {
public:
    std::string op;
    ExprNode *arg;
    ~UnaryOpNode();
    UnaryOpNode(std::string op, ASTNode *arg);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class BinaryOpNode : public ExprNode {
public:
    std::string op;
    ExprNode *arg1, *arg2;
    ~BinaryOpNode();
    BinaryOpNode(std::string op, ASTNode *arg1, ASTNode *arg2);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class BlockNode : public ExprNode {
public:
    std::vector<StmtNode*> stmts;
    ~BlockNode();
    BlockNode(std::vector<ASTNode*> &stmts);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class AssignNode : public ExprNode {
public:
    ExprNode *lhs, *rhs;
    ~AssignNode();
    AssignNode(ASTNode *lhs, ASTNode *rhs);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class AddressNode : public ExprNode {
public:
    ExprNode *addr;
    ExprNode *offset;
    ~AddressNode();
    AddressNode(ASTNode *addr);
    AddressNode(ASTNode *addr, ASTNode *offset);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class AccessNode : public ExprNode {
public:
    ExprNode *addr;
    std::string field;
    ~AccessNode();
    AccessNode(ASTNode *addr, std::string field);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class IfNode : public ExprNode {
public:
    ExprNode *cond;
    StmtNode *body_then, *body_else;
    ~IfNode();
    IfNode(ASTNode *cond, ASTNode *body_then);
    void set_else(ASTNode *body_else);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class CaseNode : public ASTNode {
public:
    bool literal;
    ASTNode *cond;
    StmtNode *body;
    ~CaseNode();
    CaseNode(bool literal, ASTNode *cond, ASTNode *body);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class MatchNode : public ExprNode {
public:
    ExprNode *expr;
    std::vector<CaseNode*> cases;
    StmtNode *deflt;
    ~MatchNode();
    MatchNode(ASTNode *expr, std::vector<ASTNode*> &cases, ASTNode *deflt);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class DeconsNode : public ASTNode {
public:
    std::string id;
    std::vector<IDNode*> fields;
    ~DeconsNode();
    DeconsNode(std::string id, std::vector<ASTNode*> &fields);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class MallocNode : public ExprNode {
public:
    TypeNode *type;
    ExprNode *init;
    ExprNode *n;
    ~MallocNode();
    MallocNode(ASTNode *type, ASTNode *init, ASTNode *n);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

class CallNode : public ExprNode {
public:
    std::string id;
    std::vector<ExprNode*> args;
    ~CallNode();
    CallNode(std::string id, std::vector<ASTNode*> &args);
    std::ostream& show(std::ostream &out);
    llvm::Value *codegen(CodeGenerator &generator) { return generator.codegen(this); };
};

#endif