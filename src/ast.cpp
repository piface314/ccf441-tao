#include <iostream>
#include "ast.hpp"
#include "parser.hpp"
#include "symtable.hpp"
#include "colors.hpp"

extern SymTable *env;
extern YYLTYPE yylloc;
extern std::vector<ASTNode*> empty;
extern std::vector<StmtNode*> loops;
extern std::vector<CallableNode*> procs;

VarTypeNode char_type("Char", empty);
VarTypeNode int_type("Int", empty);
VarTypeNode real_type("Real", empty);
VarTypeNode bool_type("Bool", empty);

std::vector<std::pair<std::string,std::string>> coercion = {
    std::make_pair("Int", "Char"),
    std::make_pair("Real", "Int"),
    std::make_pair("Bool", "Int"),
    std::make_pair("Bool", "Char")
};

std::string types_str(std::vector<TypeNode*>ts) {
    std::string p;
    for (auto it = ts.begin(); it != ts.end(); ++it)
        p += (it == ts.end()-1 ? (*it)->str() : (*it)->str() + ",");
    return "(" + p + ")";
}

std::ostream &serr() {
    return std::cerr << CL_FG_RED CL_BOLD << yylloc.first_line << ":" << yylloc.first_column
        << ": semantic unbalance: " CL_RESET;
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
    this->expr_type = &int_type;
}
LiteralNode::LiteralNode(double x) {
    this->type_tag = REAL;
    this->val.x = x;
    this->expr_type = &real_type;
}
LiteralNode::LiteralNode(char c) {
    this->type_tag = CHAR;
    this->val.c = c;
    this->expr_type = &char_type;
}
LiteralNode::LiteralNode(std::string s) {
    this->type_tag = STRING;
    new (&this->val.s) std::string(s);
    this->expr_type = new PtrTypeNode(s.size()+1, &char_type);
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

IDNode::IDNode(std::string id, bool chk) {
    this->id = id;
    if (!chk || !env) return;
    std::vector<SymTableEntry> *es = env->lookup(id);
    if (!es)
        { serr() << "`" << id << "` not in scope" << std::endl; exit(1); }
    this->expr_type = es->back().node->get_type();
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
TypeChk TypeParamNode::check(TypeNode *node) {
    return TypeChk::INC;
}
std::string TypeParamNode::str() {
    return id;
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
TypeChk PtrTypeNode::check(TypeNode *node) {
    PtrTypeNode *pnode = dynamic_cast<PtrTypeNode*>(node);
    if (!pnode) return TypeChk::INC;
    return this->type->check(pnode->type);
}
std::string PtrTypeNode::str() {
    return (size ? "[" + std::to_string(size) + "]" : "@") + type->str();
}

VarTypeNode::~VarTypeNode() { VDEL(params); }
VarTypeNode::VarTypeNode(std::string id, std::vector<ASTNode *> &params) {
    this->id = id;
    VCOPY(TypeNode,params);
    if (!env) return;
    std::vector<SymTableEntry> *es = env->lookup(id);
    if (!es)
        { serr() << "type `" << id << "` not in scope" << std::endl; exit (1); }
    for (auto &it: *es)
        if (it.node->declares_type())
            return;
    serr() << "`" << id << "` is not a type\n"; exit(1);
}
std::ostream& VarTypeNode::show(std::ostream &out) {
    out << "VarType(" << id;
    if (!params.empty()) {
        out << ", ";
        VSHOW(params);
    }
    return out << ")";
}
TypeChk VarTypeNode::check(TypeNode *node) {
    VarTypeNode *vnode = dynamic_cast<VarTypeNode*>(node);
    if (!vnode) return TypeChk::INC;
    if (id == vnode->id) return TypeChk::EQ;
    for (auto &it: coercion)
        if (id == it.first && vnode->id == it.second)
            return TypeChk::CMP;
    return TypeChk::INC;
}
std::string VarTypeNode::str() {
    if (params.empty())
        return id;
    std::string p;
    for (auto it = params.begin(); it != params.end(); ++it)
        p += (it == params.end()-1 ? (*it)->str() : (*it)->str() + ",");
    return id + "(" + p + ")";
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
TypeChk FunTypeNode::check(TypeNode *node) {
    FunTypeNode *fnode = dynamic_cast<FunTypeNode*>(node);
    if (!fnode) return TypeChk::INC;
    if (params.size() != fnode->params.size()) return TypeChk::INC;
    TypeChk chk = TypeChk::EQ;
    int i = 0;
    for (auto &p : params) {
        TypeChk c = p->check(fnode->params[i]);
        if (c == TypeChk::INC) return TypeChk::INC;
        else if (c == TypeChk::CMP) chk = c;
        ++i;
    }
    if (ret->check(fnode->ret) != TypeChk::EQ)
        return TypeChk::INC;
    return chk;
}
std::string FunTypeNode::str() {
    return types_str(params) + " |||::| " + ret->str();
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
TypeChk ProcTypeNode::check(TypeNode *node) {
    ProcTypeNode *pnode = dynamic_cast<ProcTypeNode*>(node);
    if (!pnode) return TypeChk::INC;
    if (params.size() != pnode->params.size()) return TypeChk::INC;
    TypeChk chk = TypeChk::EQ;
    int i = 0;
    for (auto &p : params) {
        TypeChk c = p->check(pnode->params[i]);
        if (c == TypeChk::INC) return TypeChk::INC;
        else if (c == TypeChk::CMP) chk = c;
        ++i;
    }
    return chk;
}
std::string ProcTypeNode::str() {
    return types_str(params) + " |||:||";
}

YinNode::~YinNode() { delete init; }
YinNode::YinNode(std::string id, ASTNode *type, ASTNode *init) {
    this->id = id;
    COPY(TypeNode,type);
    COPY(ExprNode,init);
    if (!init) return;
    TypeNode *t = this->init->get_type();
    if (!t)
        { serr() << "no value for initialization" << std::endl; exit(1); }
    if (!this->type->check(t)) {
        serr() << "invalid initialization for `" << id << ": " << this->type->str()
            << "` from type " << t->str() << std::endl;
        exit(1);
    }
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
    TypeNode *t = this->body->get_type();
    if (!t) {
        serr() << "no type for `" << id << "` expression (expected "
            << ret->str() << ")" << std::endl;
        exit(1);
    }
    if (!ret->check(t)) {
        serr() << "incompatible type " << t->str() << " for `" << id << "` expression (expected "
            << ret->str() << ")" << std::endl;
        exit(1);
    }
}
std::ostream& YangNode::show(std::ostream &out) {
    out << "Yang(";
    if (!t_params.empty()) { VSHOW(t_params); out << ", "; }
    out << id << ", ";
    VSHOW(params);
    return out << ", " << *ret << ", " << *body << ")";
}
TypeNode *YangNode::get_type() {
    if (type) return type;
    std::vector<ASTNode*> p;
    for (auto &it: params)
        p.push_back(it->type);
    type = new FunTypeNode(p, ret);
    return type;
}

WujiNode::~WujiNode() { VDEL(t_params); VDEL(params); delete body; }
WujiNode::WujiNode(std::string id, std::vector<ASTNode*> &params) {
    this->id = id;
    this->ret = NULL;
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
    // verificar se há definiçãor recursiva de tipo
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
    // verificar se há definiçãor recursiva de tipo
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
    TypeNode *t = this->cond->get_type();
    if (!t)
        { serr() << "no value for :|| condition"; exit(1); }
    if (!bool_type.check(t)) {
        serr() << "type " << t->str() << " incompatible for :|| condition"
            << " (expected Bool)" << std::endl; 
        exit(1);
    }
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
    TypeNode *t = this->cond->get_type();
    if (!t)
        { serr() << "no value for :||:|| condition"; exit(1); }
    if (!bool_type.check(t)) {
        serr() << "type " << t->str() << " incompatible for :||:|| condition"
            << " (expected Bool)" << std::endl; 
        exit(1);
    }
}
std::ostream& RepeatNode::show(std::ostream &out) {
    out << "Repeat(" << *body << ", " << *cond;
    return (step ? out << ", " << *step : out) << ")"; 
}

BreakNode::BreakNode() {
    if (loops.empty())
        { serr() << ":||||: must be inside a loop" << std::endl; exit(1); }
    loop = loops.back();
}
std::ostream& BreakNode::show(std::ostream &out) {
    return out << "Break()";
}

ContinueNode::ContinueNode() {
    if (loops.empty())
        { serr() << ":||:|: must be inside a loop" << std::endl; exit(1); }
    loop = loops.back();
}
std::ostream& ContinueNode::show(std::ostream &out) {
    return out << "Continue()";
}

ReturnNode::~ReturnNode() { delete expr; }
ReturnNode::ReturnNode(ASTNode *expr) {
    COPY(ExprNode,expr);
    if (procs.empty())
        { serr() << "|||||: must be inside yang or wuji" << std::endl; exit(1); }
    proc = procs.back();
    TypeNode *ret = proc->ret;
    if (ret) {
        if (!expr)
            { serr() << "|||||: must return a value for yang" << std::endl; exit(1); }
        TypeNode *t = this->expr->get_type();
        if (!t)
            { serr() << "|||||: must return a value for yang" << std::endl; exit(1); }
        if (!ret->check(t)) {
            serr() << "incompatible return type " << t->str()
                << " (expected " << ret->str() << ")" << std::endl;
            exit(1);
        }
    } else if (expr)
        { serr() << "|||||: cannot return a value for wuji" << std::endl; exit(1); }
}
std::ostream& ReturnNode::show(std::ostream &out) {
    out << "Return(";
    if (expr) out << *expr;
    return out << ")";
}

FreeNode::~FreeNode() { delete expr; }
FreeNode::FreeNode(ASTNode *expr) {
    COPY(ExprNode,expr);
    PtrTypeNode *pt = dynamic_cast<PtrTypeNode*>(this->expr->get_type());
    if (!pt)
        { serr() << "cannot free memory from non pointer" << std::endl; exit(1); }
}
std::ostream& FreeNode::show(std::ostream &out) {
    return out << "Free(" << *expr << ")";
}

RefNode::~RefNode() { delete expr; }
RefNode::RefNode(ASTNode *expr) {
    COPY(ExprNode,expr);
    this->expr_type = new PtrTypeNode(0, this->expr->get_type());
}
std::ostream& RefNode::show(std::ostream &out) {
    return out << "Ref(" << *expr << ")";
}

UnaryOpNode::~UnaryOpNode() { delete arg; }
UnaryOpNode::UnaryOpNode(char op, ASTNode *arg) {
    this->op = op;
    COPY(ExprNode,arg);
    std::vector<SymTableEntry> *es = env->lookup_all(std::string(1,op));
    if (!es)
        { serr() << "operator " << op << " not in scope" << std::endl; exit(1); }
    TypeNode *t1 = this->arg->get_type();
    if (!t1)
        { serr() << "no value for operand" << std::endl; exit(1); }
    TypeNode *ret = NULL;
    for (auto &it: *es) {
        FunTypeNode *t = dynamic_cast<FunTypeNode*>(it.node->get_type());
        if (t->params.size() != 1) continue;
        TypeChk chk = t->params[0]->check(t1);
        if (chk == TypeChk::EQ) {
            ret = t->ret;
            break;
        } else if (chk != TypeChk::INC && !ret)
            ret = t->ret;
    }
    if (!ret) { 
        serr() << "no compatible operator " << op << " for argument type "
            << t1->str() << std::endl;
        exit(1);
    }
    this->expr_type = ret;
    delete es;
}
std::ostream& UnaryOpNode::show(std::ostream &out) {
    return out << "UnaryOp(" << op << ", " << *arg << ")";
}

BinaryOpNode::~BinaryOpNode() { delete arg1; delete arg2; }
BinaryOpNode::BinaryOpNode(std::string op, ASTNode *arg1, ASTNode *arg2) {
    this->op = op;
    COPY(ExprNode,arg1);
    COPY(ExprNode,arg2);
    std::vector<SymTableEntry> *es = env->lookup_all(op);
    if (!es)
        { serr() << "operator " << op << " not in scope" << std::endl; exit(1); }
    TypeNode *t1 = this->arg1->get_type();
    TypeNode *t2 = this->arg2->get_type();
    if (!t1)
        { serr() << "no value for first operand" << std::endl; exit(1); }
    if (!t2)
        { serr() << "no value for second operand" << std::endl; exit(1); }
    TypeNode *ret = NULL;
    for (auto &it: *es) {
        FunTypeNode *t = dynamic_cast<FunTypeNode*>(it.node->get_type());
        if (t->params.size() != 2) continue;
        TypeChk chk1 = t->params[0]->check(t1);
        TypeChk chk2 = t->params[1]->check(t2);
        if (chk1 == TypeChk::EQ && chk2 == TypeChk::EQ) {
            ret = t->ret;
            break;
        } else if (chk1 && chk2 && !ret)
            ret = t->ret;
    }
    if (!ret) { 
        serr() << "no compatible operator " << op << " for argument types "
            << t1->str() << " and " << t2->str() << std::endl;
        exit(1);
    }
    this->expr_type = ret;
    delete es;
}
std::ostream& BinaryOpNode::show(std::ostream &out) {
    return out << "BinaryOp(" << op << ", " << *arg1 << ", " << *arg2 << ")";
}

BlockNode::~BlockNode() { VDEL(stmts); }
BlockNode::BlockNode(std::vector<ASTNode*> &stmts) {
    VCOPY(StmtNode,stmts);
    if (this->stmts.empty()) return;
    this->expr_type = this->stmts.back()->get_type();
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
    TypeNode *tl = this->lhs->get_type();
    TypeNode *tr = this->rhs->get_type();
    if (!tr)
        { serr() << "no value to assign" << std::endl; exit(1); }
    if (!tl->check(tr)) {
        serr() << "invalid type " << tr->str() << " for assignment "
            << "(expected " << tl->str() << ")" << std::endl; exit(1);
    }
    this->expr_type = tr;
}
std::ostream& AssignNode::show(std::ostream &out) {
    return out << "Assign(" << *lhs << ", " << *rhs << ")";
}

AddressNode::~AddressNode() { delete addr; delete offset; }
AddressNode::AddressNode(ASTNode *addr, ASTNode *offset) {
    COPY(ExprNode,addr);
    COPY(ExprNode,offset);
    PtrTypeNode *pt = dynamic_cast<PtrTypeNode*>(this->addr->get_type());
    if (!pt) 
        { serr() << "cannot address non pointer value" << std::endl; exit(1); }
    if (this->offset) {
        TypeNode *t = this->offset->get_type();
        if (!int_type.check(t))
            { serr() << "cannot address pointer with non integer value" << std::endl; exit(1); }
    }
    this->expr_type = pt->type;
}
std::ostream& AddressNode::show(std::ostream &out) {
    out << "Addr(" << *addr << ", ";
    if (offset) out << *offset; else out << "@";
    return out << ")";
}

AccessNode::~AccessNode() { delete addr; }
AccessNode::AccessNode(ASTNode *addr, std::string field) {
    // verificar se o tipo de addr contém o campo field
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
    TypeNode *t = this->cond->get_type();
    if (!t)
        { serr() << "no value for :|: condition"; exit(1); }
    if (!bool_type.check(t)) {
        serr() << "type " << t->str() << " incompatible for :|: condition"
            << " (expected Bool)" << std::endl; 
        exit(1);
    }
}
void IfNode::set_else(ASTNode *body_else) {
    COPY(StmtNode,body_else);
    if (!body_else) return;
    TypeNode *tt = this->body_then->get_type();
    TypeNode *te = this->body_else->get_type();
    if (!tt || !te) return;
    if (tt->check(te))
        this->expr_type = tt;
}
std::ostream& IfNode::show(std::ostream &out) {
    out << "If(" << *cond << ", " << *body_then;
    if (body_else) out << ", " << *body_else;
    return out << ")";
}

CaseNode::~CaseNode() { delete cond; delete body; }
CaseNode::CaseNode(bool literal, ASTNode *cond, ASTNode *body) {
    this->literal = literal;
    COPY(ExprNode,cond);
    COPY(StmtNode,body);
    this->expr_type = this->body->get_type();
}
std::ostream& CaseNode::show(std::ostream &out) {
    return out << "Case(" << *cond << ", " << *body << ")";
}

MatchNode::~MatchNode() { delete expr; VDEL(cases); delete deflt; }
MatchNode::MatchNode(ASTNode *expr, std::vector<ASTNode*> &cases, ASTNode *deflt) {
    COPY(ExprNode,expr);
    VCOPY(CaseNode,cases);
    COPY(StmtNode,deflt);
    // verificar se expr e todos os cases tem o mesmo tipo
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
    // se existe o construtor de tipo referido
    // verificar se os campos batem
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
    if (this->n && !int_type.check(this->n->get_type())) {
        serr() << "cannot allocate non integer amount of memory" << std::endl;
        exit(1);
    }
    if (this->init) {
        TypeNode *t = this->init->get_type();
        if (!t)
            { serr() << "no value to infer allocation type" << std::endl; exit(1); }
        this->expr_type = new PtrTypeNode(0, t);
    } else
        this->expr_type = new PtrTypeNode(0, this->type);
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
    std::vector<SymTableEntry> *es = env->lookup_all(id);
    if (!es)
        { serr() << "function " << id << " not in scope" << std::endl; exit(1); }
    std::vector<TypeNode*> targs;
    int i = 0;
    for (auto &it : this->args) {
        ++i;
        TypeNode *targ = it->get_type();
        if (!targ) {
            serr() << "no value for `" << id << "` argument " << i << std::endl;
            exit(1);
        }
        targs.push_back(targ);
    }
    TypeNode *ret = NULL;
    for (auto &it: *es) {
        CallableNode *cnode = dynamic_cast<CallableNode*>(it.node);
        if (cnode->params.size() != args.size()) continue;
        TypeChk chk = TypeChk::EQ;
        for (size_t i = 0; i < args.size(); ++i) {
            TypeNode *tp = cnode->params[i]->get_type();
            TypeNode *ta = targs[i]; 
            TypeChk c = tp->check(ta);
            if (!c) { chk = c; break; }
            if (c == TypeChk::CMP) chk = c;
        }
        if (chk == TypeChk::EQ) { ret = cnode->ret; break; }
        if (chk && !ret) ret = cnode->ret;
    }
    if (!ret) {
        serr() << "no compatible definition of `" << id << "` for argument types "
            << types_str(targs) << std::endl;
        exit(1);
    }
    this->expr_type = ret;
    delete es;
}
std::ostream& CallNode::show(std::ostream &out) {
    out << "Call(" << id;
    VSHOW(args);
    return out << ")";
}