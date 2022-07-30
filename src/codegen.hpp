#ifndef CODEGEN_H
#define CODEGEN_H

#include <iostream>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include "defs.hpp"

static llvm::LLVMContext ctx;
static llvm::IRBuilder<> builder(ctx);
static std::unique_ptr<llvm::Module> llvm_module;
static std::map<std::string, llvm::Value*> named_vals;


#endif