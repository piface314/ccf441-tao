#include <iostream>
#include "ast.hpp"
#include "parser.hpp"
#include "symtable.hpp"

extern SymTable *env;
extern YYLTYPE yylloc;

std::ostream &serror() {
    return std::cerr << yylloc.first_line << ":" << yylloc.first_column << ": semantic unbalance: ";
}

std::ostream& operator<<(std::ostream& os, ASTNode& node) {
    return node.show(os);
}

std::ostream& ASTNode::show(std::ostream &out) {
    return out << "ASTNode()";
}

LiteralNode::LiteralNode(int n) {
    this->type_tag = INTEGER;
    this->val.n = n;
}
LiteralNode::LiteralNode(double x) {
    this->type_tag = REAL;
    this->val.x = x;
}
LiteralNode::LiteralNode(char c) {
    this->type_tag = CHAR;
    this->val.c = c;
}
LiteralNode::LiteralNode(std::string s) {
    this->type_tag = STRING;
    new (&this->val.s) std::string(s);
}
std::ostream& LiteralNode::show(std::ostream &out) {
    out << "Literal(";
    switch (type_tag) {
    case INTEGER: out << val.n << ")"; break;
    case REAL: out << val.x << ")"; break;
    case CHAR: out << "'" << val.c << "')"; break;
    case STRING: out << "\"" << val.s << "\")"; break;
    default: out << ")"; break;
    }
    return out;
}

IDNode::IDNode(std::string id) {
    this->id = id;
    if (!env) return;
    std::stack<SymTableEntry> *e = env->lookup(id);
    if (!e)
        { serror() << "`" << id << "` not in scope" << std::endl; exit (1); }
}
std::ostream& IDNode::show(std::ostream &out) {
    return out << "ID(" << id << ")";
}

ExportNode::~ExportNode() { VDEL(exports); }
ExportNode::ExportNode(std::string id, std::vector<ASTNode*> &exports) {
    this->id = id;
    VCOPY(IDNode,exports);
}
std::ostream& ExportNode::show(std::ostream &out) {
    out << "Export(" << id << ", ";
    VSHOW(exports);
    return out << ")";
}

ProgramNode::~ProgramNode() { VDEL(stmts); }
ProgramNode::ProgramNode(ASTNode *module_decl, std::vector<ASTNode*> &stmts) {
    COPY(ExportNode,module_decl);
    VCOPY(StmtNode,stmts);
}
std::ostream& ProgramNode::show(std::ostream &out) {
    out << "Program(";
    if (module_decl) out << *module_decl << ", ";
    VSHOW(stmts);
    return out << ")";
}

ImportNode::~ImportNode() { VDEL(imports); }
ImportNode::ImportNode(std::string id, std::vector<ASTNode*> &imports, std::string alias) {
    this->id = id;
    VCOPY(IDNode,imports);
    this->alias = alias;
}
std::ostream& ImportNode::show(std::ostream &out) {
    out << "Import(" << id << ", ";
    if (!alias.empty())
        out << alias << ", ";
    VSHOW(imports);
    return out << ")";
}

ParamNode::~ParamNode() { delete type; }
ParamNode::ParamNode(std::string id, ASTNode *type) {
    this->id = id;
    COPY(TypeNode,type);
}
std::ostream& ParamNode::show(std::ostream &out) {
    return out << "Param(" << id << ", " << *type << ")";
}

TypeParamNode::~TypeParamNode() {}
TypeParamNode::TypeParamNode(std::string id) {
    this->id = id;
}
std::ostream& TypeParamNode::show(std::ostream &out) {
    return out << "TypeParam(" << id << ")";
}

PtrTypeNode::~PtrTypeNode() { delete type; }
PtrTypeNode::PtrTypeNode(int size) {
    this->size = size;
    this->type = NULL;
}
PtrTypeNode::PtrTypeNode(int size, TypeNode *type) {
    this->size = size;
    this->type = type;
}
void PtrTypeNode::set_type(ASTNode *type) {
    COPY(TypeNode,type);
}
std::ostream& PtrTypeNode::show(std::ostream &out) {
    out << "Ptr(";
    if (size) out << size; else out << "@";
    return out << ", " << *type << ")";
}

VarTypeNode::~VarTypeNode() { VDEL(params); }
VarTypeNode::VarTypeNode(std::string id, std::vector<ASTNode *> &params) {
    this->id = id;
    VCOPY(TypeNode,params);
    if (!env) return;
    std::stack<SymTableEntry> *e = env->lookup(id);
    if (!e)
        { serror() << "type `" << id << "` not in scope" << std::endl; exit (1); }
    if (!e->top().node->declares_type())
        { serror() << "`" << id << "` is not a type\n"; exit(1); }
}
std::ostream& VarTypeNode::show(std::ostream &out) {
    out << "VarType(" << id;
    if (!params.empty()) {
        out << ", ";
        VSHOW(params);
    }
    return out << ")";
}

FunTypeNode::~FunTypeNode() { VDEL(params); delete ret; }
FunTypeNode::FunTypeNode(std::vector<ASTNode *> &params, ASTNode *ret) {
    VCOPY(TypeNode,params);
    COPY(TypeNode,ret);
}
std::ostream& FunTypeNode::show(std::ostream &out) {
    out << "FunType(";
    VSHOW(params);
    return out << ", " << *ret << ")";
}

ProcTypeNode::~ProcTypeNode() { VDEL(params); }
ProcTypeNode::ProcTypeNode(std::vector<ASTNode *> &params) {
    VCOPY(TypeNode,params);
}
std::ostream& ProcTypeNode::show(std::ostream &out) {
    out << "ProcType(";
    VSHOW(params);
    return out << ")";
}

YinNode::~YinNode() { delete init; }
YinNode::YinNode(std::string id, ASTNode *type, ASTNode *init) {
    this->id = id;
    COPY(TypeNode,type);
    COPY(ExprNode,init);
}
std::ostream& YinNode::show(std::ostream &out) {
    out << "Yin(" << id << ", " << *type;
    if (init)
        out << ", " << *init;
    return out << ")";
}

YangNode::~YangNode() { VDEL(t_params); delete ret; delete body; }
YangNode::YangNode(std::string id, std::vector<ASTNode*> &params, ASTNode *ret) {
    this->id = id;
    VCOPY(ParamNode,params);
    COPY(TypeNode,ret);
}
void YangNode::set_type_params(std::vector<ASTNode*> &t_params) {
    VCOPY(TypeParamNode,t_params);
}
void YangNode::set_body(ASTNode *body) {
    COPY(ExprNode,body);
}
std::ostream& YangNode::show(std::ostream &out) {
    out << "Yang(";
    if (!t_params.empty()) { VSHOW(t_params); out << ", "; }
    out << id << ", ";
    VSHOW(params);
    return out << ", " << *ret << ", " << *body << ")";
}

WujiNode::~WujiNode() { VDEL(t_params); VDEL(params); delete body; }
WujiNode::WujiNode(std::string id, std::vector<ASTNode*> &params) {
    this->id = id;
    VCOPY(ParamNode,params);
}
void WujiNode::set_type_params(std::vector<ASTNode*> &t_params) {
    VCOPY(TypeParamNode,t_params);
}
void WujiNode::set_body(ASTNode *body) {
    COPY(ExprNode,body);
}
std::ostream& WujiNode::show(std::ostream &out) {
    out << "Wuji(";
    if (!t_params.empty()) { VSHOW(t_params); out << ", "; }
    out << id << ", ";
    VSHOW(params);
    return out << ", " << *body << ")";
}

ConstrNode::~ConstrNode() { VDEL(params); delete ret; }
ConstrNode::ConstrNode(std::string id, std::vector<ASTNode *> &params) {
    this->id = id;
    VCOPY(ParamNode,params);
}
std::ostream& ConstrNode::show(std::ostream &out) {
    out << "Constr(" << id << ", ";
    VSHOW(params);
    return out << ")";
}

TypeDefNode::~TypeDefNode() { VDEL(params); VDEL(constrs); }
TypeDefNode::TypeDefNode(std::string id, std::vector<ASTNode*> &params) {
    this->id = id;
    VCOPY(TypeParamNode,params);
}
void TypeDefNode::add_constrs(std::vector<ASTNode*> &constrs) {
    VCOPY(ConstrNode,constrs);
    for (auto it = this->constrs.begin(); it != this->constrs.end(); ++it)
        (*it)->ret = get_type();
}
TypeNode *TypeDefNode::get_type() {
    std::vector<ASTNode*> p;
    int i = 0;
    for (auto it = params.begin(); it != params.end(); ++it, ++i)
        p.push_back(params[i]);
    return new VarTypeNode(id, p);
}
std::ostream& TypeDefNode::show(std::ostream &out) {
    out << "TypeDef(" << id << ", ";
    VSHOW(params); out << ", ";
    VSHOW(constrs);
    return out << ")";
}

TypeAliasNode::~TypeAliasNode() { VDEL(params); delete type; }
TypeAliasNode::TypeAliasNode(std::string id, std::vector<ASTNode*> &params, ASTNode *type) {
    this->id = id;
    VCOPY(TypeParamNode,params);
    COPY(TypeNode,type);
}
std::ostream& TypeAliasNode::show(std::ostream &out) {
    out << "TypeAlias(" << id << ", ";
    VSHOW(params);
    return out << ", " << *type << ")";
}

WhileNode::~WhileNode() { delete cond; delete step; delete body; }
WhileNode::WhileNode(ASTNode *cond, ASTNode *step, ASTNode *body) {
    COPY(ExprNode,cond);
    COPY(StmtNode,step);
    COPY(StmtNode,body);
}
std::ostream& WhileNode::show(std::ostream &out) {
    out << "While(" << *cond << ", ";
    if (step) out << *step << ", ";
    return out << *body << ")";
}

RepeatNode::~RepeatNode() { delete body; delete cond; delete step; }
RepeatNode::RepeatNode(ASTNode *body, ASTNode *cond, ASTNode *step) {
    COPY(StmtNode,body);
    COPY(ExprNode,cond);
    COPY(StmtNode,step);
}
std::ostream& RepeatNode::show(std::ostream &out) {
    out << "Repeat(" << *body << ", " << *cond;
    return (step ? out << ", " << *step : out) << ")"; 
}

std::ostream& BreakNode::show(std::ostream &out) {
    return out << "Break()";
}

std::ostream& ContinueNode::show(std::ostream &out) {
    return out << "Continue()";
}

ReturnNode::~ReturnNode() { delete expr; }
ReturnNode::ReturnNode(ASTNode *expr) {
    COPY(ExprNode,expr);
}
std::ostream& ReturnNode::show(std::ostream &out) {
    out << "Return(";
    if (expr) out << *expr;
    return out << ")";
}

FreeNode::~FreeNode() { delete expr; }
FreeNode::FreeNode(ASTNode *expr) {
    COPY(ExprNode,expr);
}
std::ostream& FreeNode::show(std::ostream &out) {
    return out << "Free(" << *expr << ")";
}

UnaryOpNode::~UnaryOpNode() { delete arg; }
UnaryOpNode::UnaryOpNode(std::string op, ASTNode *arg) {
    this->op = op;
    COPY(ExprNode,arg);
}
std::ostream& UnaryOpNode::show(std::ostream &out) {
    return out << "UnaryOp(" << op << ", " << *arg << ")";
}

BinaryOpNode::~BinaryOpNode() { delete arg1; delete arg2; }
BinaryOpNode::BinaryOpNode(std::string op, ASTNode *arg1, ASTNode *arg2) {
    this->op = op;
    COPY(ExprNode,arg1);
    COPY(ExprNode,arg2);
}
std::ostream& BinaryOpNode::show(std::ostream &out) {
    return out << "BinaryOp(" << op << ", " << *arg1 << ", " << *arg2 << ")";
}

BlockNode::~BlockNode() { VDEL(stmts); }
BlockNode::BlockNode(std::vector<ASTNode*> &stmts) {
    VCOPY(StmtNode,stmts);
}
std::ostream& BlockNode::show(std::ostream &out) {
    out << "Block(";
    VSHOW(stmts);
    return out << ")";
}

AssignNode::~AssignNode() { delete lhs; delete rhs;}
AssignNode::AssignNode(ASTNode *lhs, ASTNode *rhs) {
    COPY(ExprNode,lhs);
    COPY(ExprNode,rhs);
}
std::ostream& AssignNode::show(std::ostream &out) {
    return out << "Assign(" << *lhs << ", " << *rhs << ")";
}

AddressNode::~AddressNode() { delete addr; delete offset; }
AddressNode::AddressNode(ASTNode *addr) {
    COPY(ExprNode,addr);
    this->offset = NULL;
}
AddressNode::AddressNode(ASTNode *addr, ASTNode *offset) {
    COPY(ExprNode,addr);
    COPY(ExprNode,offset);
}
std::ostream& AddressNode::show(std::ostream &out) {
    out << "Addr(" << *addr << ", ";
    if (offset) out << "@"; else out << *offset;
    return out << ")";
}

AccessNode::~AccessNode() { delete addr; }
AccessNode::AccessNode(ASTNode *addr, std::string field) {
    COPY(ExprNode,addr);
    this->field = field;
}
std::ostream& AccessNode::show(std::ostream &out) {
    return out << "Access(" << *addr << ", " << field << ")";
}

IfNode::~IfNode() { delete cond; delete body_then; delete body_else; }
IfNode::IfNode(ASTNode *cond, ASTNode *body_then) {
    COPY(ExprNode,cond);
    COPY(StmtNode,body_then);
    this->body_else = NULL;
}
void IfNode::set_else(ASTNode *body_else) {
    COPY(StmtNode,body_else);
}
std::ostream& IfNode::show(std::ostream &out) {
    out << "If(" << *cond << ", " << *body_then;
    if (body_else) out << ", " << *body_else;
    return out << ")";
}

CaseNode::~CaseNode() { delete cond; delete body; }
CaseNode::CaseNode(bool literal, ASTNode *cond, ASTNode *body) {
    this->literal = literal;
    this->cond = cond;
    COPY(StmtNode,body);
}
std::ostream& CaseNode::show(std::ostream &out) {
    return out << "Case(" << *cond << ", " << *body << ")";
}

MatchNode::~MatchNode() { delete expr; VDEL(cases); delete deflt; }
MatchNode::MatchNode(ASTNode *expr, std::vector<ASTNode*> &cases, ASTNode *deflt) {
    COPY(ExprNode,expr);
    VCOPY(CaseNode,cases);
    COPY(StmtNode,deflt);
}
std::ostream& MatchNode::show(std::ostream &out) {
    out << "Match(" << *expr << ", ";
    VSHOW(cases);
    if (deflt) out << ", " << *deflt;
    return out << ")";
}

DeconsNode::~DeconsNode() { VDEL(fields); }
DeconsNode::DeconsNode(std::string id, std::vector<ASTNode*> &fields) {
    this->id = id;
    VCOPY(IDNode,fields);
}
std::ostream& DeconsNode::show(std::ostream &out) {
    out << "Decons(" << id << ", ";
    VSHOW(fields);
    return out << ")";
}

MallocNode::~MallocNode() { delete type; delete init; delete n; }
MallocNode::MallocNode(ASTNode *type, ASTNode *init, ASTNode *n) {
    COPY(TypeNode,type);
    COPY(ExprNode,init);
    COPY(ExprNode,n);
}
std::ostream& MallocNode::show(std::ostream &out) {
    out << "Malloc(";
    if (type) out << *type;
    if (init) out << *init;
    if (n) out << ", " << *n;
    return out << ")";
}

CallNode::~CallNode() { VDEL(args); }
CallNode::CallNode(std::string id, std::vector<ASTNode*> &args) {
    this->id = id;
    VCOPY(ExprNode,args);
}
std::ostream& CallNode::show(std::ostream &out) {
    out << "Call(" << id;
    VSHOW(args);
    return out << ")";
}