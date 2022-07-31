#ifndef DEFS_H
#define DEFS_H

#include <llvm/IR/Value.h>
class ASTNode;
class TypeNode;
class StmtNode;
class ExprNode;
class LiteralNode;
class IDNode;
class ExportNode;
class ProgramNode;
class ImportNode;
class ParamNode;
class TypeParamNode;
class PtrTypeNode;
class VarTypeNode;
class FunTypeNode;
class ProcTypeNode;
class YinNode;
class CallableNode;
class YangNode;
class WujiNode;
class ConstrNode;
class TypeDefNode;
class TypeAliasNode;
class WhileNode;
class RepeatNode;
class BreakNode;
class ContinueNode;
class ReturnNode;
class FreeNode;
class RefNode;
class UnaryOpNode;
class BinaryOpNode;
class BlockNode;
class AssignNode;
class AddressNode;
class AccessNode;
class IfNode;
class CaseNode;
class MatchNode;
class DeconsNode;
class MallocNode;
class CallNode;

class CodeGenerator {
public:
    llvm::Value *codegen(ASTNode *node);
    llvm::Value *codegen(ProgramNode *node);
};

#endif