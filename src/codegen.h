#pragma once

#include <map>
#include <string>
#include <memory>
#include <expected>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "ast.h"

namespace lev::codegen {
  using namespace lev::ast;

  struct InvalidUnaryType {

  };

  struct Unimplemented {

  };

  using CodegenError = std::variant<InvalidUnaryType, Unimplemented>;

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
      auto codegen(const Stmt&) -> std::expected<bool, CodegenError>;
      auto codegen(const Expr&) -> std::expected<llvm::Value*, CodegenError>;

      auto visit(const Stmt::ExprStmt&) -> std::expected<bool, CodegenError>;
      auto visit(const Stmt::BlockStmt&) -> std::expected<bool, CodegenError>;
      auto visit(const Stmt::AssignStmt&) -> std::expected<bool, CodegenError>;
      auto visit(const Stmt::FunctionDeclarationStmt&) -> std::expected<bool, CodegenError>;
      auto visit(const Stmt::VariableDeclarationStmt&) -> std::expected<bool, CodegenError>;

      auto visit(const Expr::LiteralExpr&) -> std::expected<llvm::Value*, CodegenError>;
      auto visit(const Expr::BinaryExpr&) -> std::expected<llvm::Value*, CodegenError>;
      auto visit(const Expr::UnaryExpr&) -> std::expected<llvm::Value*, CodegenError>;
      auto visit(const Expr::VariableExpr&) -> std::expected<llvm::Value*, CodegenError>;

      auto convertType(ast::Type type) const -> llvm::Type*;

      auto inspectExprType(const Expr&) const -> std::expected<ast::Type, CodegenError>;
      auto inspectBinaryExprType(const Expr::BinaryExpr&) const -> std::expected<ast::Type, CodegenError>;
      auto inspectLiteralExprType(const Expr::LiteralExpr&) const -> std::expected<ast::Type, CodegenError>;
      auto inspectUnaryExprType(const Expr::UnaryExpr&) const -> std::expected<ast::Type, CodegenError>;
      auto inspectVariableExprType(const Expr::VariableExpr&) const -> std::expected<ast::Type, CodegenError>;
  };

}
