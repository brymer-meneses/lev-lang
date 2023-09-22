#pragma once

#include "ast.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace lev::codegen {
  using namespace llvm;

  static std::unique_ptr<LLVMContext> TheContext = std::make_unique<llvm::LLVMContext>();
  static std::unique_ptr<Module> TheModule = std::make_unique<llvm::Module>("lev", *TheContext);
}
