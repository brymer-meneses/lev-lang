#pragma once

#include <map>
#include <string>
#include <memory>
#include <expected>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "ast.h"
#include "context.h"

namespace lev::codegen {
  using namespace lev::ast;
  using namespace lev::semantics;

  struct InvalidUnaryType {};
  struct Unimplemented {};
  struct UndefinedVariable {};

  using CodegenError = std::variant<InvalidUnaryType, UndefinedVariable, Unimplemented>;

  class Codegen {
    private:
      std::unique_ptr<llvm::LLVMContext> mContext;
      std::unique_ptr<llvm::Module> mModule;
      std::unique_ptr<llvm::IRBuilder<>> mBuilder;

      SemanticContext mSemanticContext;
      std::map<std::string, llvm::Value*> mNamedValues;

    public:
      Codegen(std::vector<Stmt> statements);
      Codegen(std::string_view);
      
      auto compile() -> void;
      auto dump() const -> std::string;

      static auto reportErrors(CodegenError error) -> void;

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

  };

}
