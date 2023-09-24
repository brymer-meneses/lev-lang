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

  struct InvalidUnaryType {};
  struct Unimplemented {};

  using CodegenError = std::variant<InvalidUnaryType, Unimplemented>;

  class Codegen {
    private:
      std::unique_ptr<llvm::LLVMContext> mContext;
      std::unique_ptr<llvm::Module> mModule;
      std::unique_ptr<llvm::IRBuilder<>> mBuilder;

      std::map<std::string, llvm::Value*> mNamedValues;
      std::vector<Stmt> mStatements;

    public:
      Codegen(std::vector<Stmt> statements);
      Codegen(std::string_view);
      
      auto compile() -> void;
      auto dump() const -> std::string;

    private:
      auto convertType(ast::Type type) const -> llvm::Type*;

      auto codegenStmt(const Stmt&) -> std::expected<bool, CodegenError>;
      auto codegenExpr(const Expr&) -> std::expected<llvm::Value*, CodegenError>;

      auto codegen(const Stmt::ExprStmt&) -> std::expected<bool, CodegenError>;
      auto codegen(const Stmt::BlockStmt&) -> std::expected<bool, CodegenError>;
      auto codegen(const Stmt::AssignStmt&) -> std::expected<bool, CodegenError>;
      auto codegen(const Stmt::FunctionDeclarationStmt&) -> std::expected<bool, CodegenError>;
      auto codegen(const Stmt::VariableDeclarationStmt&) -> std::expected<bool, CodegenError>;

      auto codegen(const Expr::LiteralExpr&) -> std::expected<llvm::Value*, CodegenError>;
      auto codegen(const Expr::BinaryExpr&) -> std::expected<llvm::Value*, CodegenError>;
      auto codegen(const Expr::UnaryExpr&) -> std::expected<llvm::Value*, CodegenError>;
      auto codegen(const Expr::VariableExpr&) -> std::expected<llvm::Value*, CodegenError>;
      auto codegen(const Expr::CallExpr&) -> std::expected<llvm::Value*, CodegenError>;

      auto inferExprType(const Expr&) const -> std::expected<ast::Type, CodegenError>;
      auto inferStmtType(const Stmt&) const -> std::expected<ast::Type, CodegenError>;

      auto inferType(const Expr::BinaryExpr&) const -> std::expected<ast::Type, CodegenError>;
      auto inferType(const Expr::LiteralExpr&) const -> std::expected<ast::Type, CodegenError>;
      auto inferType(const Expr::UnaryExpr&) const -> std::expected<ast::Type, CodegenError>;
      auto inferType(const Expr::VariableExpr&) const -> std::expected<ast::Type, CodegenError>;
      auto inferType(const Expr::CallExpr&) const -> std::expected<ast::Type, CodegenError>;

      auto inferType(const Stmt::AssignStmt&) const -> std::expected<ast::Type, CodegenError>;
      auto inferType(const Stmt::ExprStmt&) const -> std::expected<ast::Type, CodegenError>;
      auto inferType(const Stmt::BlockStmt&) const -> std::expected<ast::Type, CodegenError>;
      auto inferType(const Stmt::VariableDeclarationStmt&) const -> std::expected<ast::Type, CodegenError>;
      auto inferType(const Stmt::FunctionDeclarationStmt&) const -> std::expected<ast::Type, CodegenError>;
  };

}
