#pragma once

#include "codegen/context.h"
#include "parsing/type.h"
#include <lev/diagnostics/errors.h>
#include <lev/parsing/ast.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <vector>
#include <expected>

namespace lev {

class Compiler {
public:
  Compiler(std::vector<Stmt> statements);

  auto dump() const -> std::string;
  auto compile() -> std::expected<void, CodegenError>;
  auto getModule() -> std::unique_ptr<llvm::Module>;

private:
  auto codegen(const Stmt::Block&) -> std::expected<void, CodegenError>;
  auto codegen(const Stmt::Return&) -> std::expected<void, CodegenError>;
  auto codegen(const Stmt::VariableDeclaration&) -> std::expected<void, CodegenError>;
  auto codegen(const Stmt::FunctionDeclaration&) -> std::expected<void, CodegenError>;
  auto codegen(const Stmt::Assignment&) -> std::expected<void, CodegenError>;
  auto codegen(const Stmt::Control&) -> std::expected<void ,CodegenError>;

  auto codegen(const Expr::Identifier&) -> std::expected<llvm::Value*, CodegenError>;
  auto codegen(const Expr::Binary&) -> std::expected<llvm::Value*, CodegenError>;
  auto codegen(const Expr::Unary&) -> std::expected<llvm::Value*, CodegenError>;
  auto codegen(const Expr::Literal&) -> std::expected<llvm::Value*, CodegenError>;

  auto codegen(const Expr&) -> std::expected<llvm::Value*, CodegenError>;
  auto codegen(const Stmt&) -> std::expected<void, CodegenError>;

  auto convertType(const LevType&) const -> llvm::Type*;

private:
  std::shared_ptr<llvm::LLVMContext> mContext;
  std::unique_ptr<llvm::Module> mModule;
  std::shared_ptr<llvm::IRBuilder<>> mBuilder;
  std::vector<Stmt> mStatements;

  Context mSemanticContext;

};

}
