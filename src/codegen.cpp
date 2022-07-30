#include "codegen.hpp"
#include "ast.hpp"

llvm::Value *CodeGenerator::codegen(ASTNode *node) {
    std::cout << "Generating code for ASTNode\n";
    return NULL;
}

llvm::Value *CodeGenerator::codegen(ProgramNode *node) {
    std::cout << "Generating code for Program\n";
    return NULL;
}