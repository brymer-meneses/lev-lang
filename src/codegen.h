#pragma once

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "ast.h"

namespace lev::codegen {
  using namespace lev::ast;

  class Codegen : StmtVisitor {
    private:
      std::unique_ptr<llvm::LLVMContext> mContext;
      std::unique_ptr<llvm::Module> mModule;
      std::unique_ptr<llvm::IRBuilder<>> mBuilder;

      llvm::Function* mCurrentFunction = nullptr;

    public:
      Codegen();
      
      auto compile(std::string_view) -> void;

      auto visit(FunctionDeclaration&) -> void final;
      auto visit(ExprStmt&) -> void final {};
      auto visit(VariableDeclaration&) -> void final;

      auto dump() -> std::string;

    private:
      auto convertType(ast::Type type) -> llvm::Type*;
  };

}
