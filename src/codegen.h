#pragma once

#include <map>
#include <string>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "ast.h"

namespace lev::codegen {
  using namespace lev::ast;

  class Codegen {
    private:
      std::unique_ptr<llvm::LLVMContext> mContext;
      std::unique_ptr<llvm::Module> mModule;
      std::unique_ptr<llvm::IRBuilder<>> mBuilder;

      std::vector<Stmt> mStatements;


    public:
      Codegen(std::vector<Stmt> statements);
      Codegen(std::string_view);
      
      auto compile() -> void;
      auto dump() const -> std::string;

    public:
      auto codegen(const Stmt&) -> void;
      auto codegen(const Expr&) -> llvm::Value*;

      auto visit(const Stmt::ExprStmt&) -> void;
      auto visit(const Stmt::BlockStmt&) -> void;
      auto visit(const Stmt::AssignStmt&) -> void;
      auto visit(const Stmt::FunctionDeclarationStmt&) -> void;
      auto visit(const Stmt::VariableDeclarationStmt&) -> void;

      auto visit(const Expr::LiteralExpr&) -> llvm::Value*;
      auto visit(const Expr::BinaryExpr&) -> llvm::Value*;
      auto visit(const Expr::UnaryExpr&) -> llvm::Value*;
      auto visit(const Expr::VariableExpr&) -> llvm::Value*;

      auto convertType(ast::Type type) const -> llvm::Type*;
  };

}
