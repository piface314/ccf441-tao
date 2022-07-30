#include "codegen.hpp"
#include "ast.hpp"

static llvm::LLVMContext ctx;
static llvm::IRBuilder<> builder(ctx);
static std::unique_ptr<llvm::Module> llvm_module;
static std::map<std::string, llvm::Value*> named_vals;

llvm::Value *CodeGenerator::codegen(ASTNode *node) {
    std::cout << "Generating code for ASTNode\n";
    return NULL;
}

llvm::Value *CodeGenerator::codegen(ProgramNode *node) {
    std::cout << "Generating code for Program\n";
    return NULL;
}
