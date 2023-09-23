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

  class Codegen : public StmtVisitor, public ExprVisitor {
    private:
      std::unique_ptr<llvm::LLVMContext> mContext;
      std::unique_ptr<llvm::Module> mModule;
      std::unique_ptr<llvm::IRBuilder<>> mBuilder;

      std::vector<std::unique_ptr<Stmt>> mStatements;

      Stmt* mCurrentStmt = nullptr;
      Expr* mCurrentExpr = nullptr;

      llvm::Value* mEvaluatedExpr = nullptr;

    public:
      Codegen(std::vector<std::unique_ptr<Stmt>> statements);
      Codegen(std::string_view);
      
      auto compile() -> void;
      auto dump() -> std::string;

    private:
      auto codegenStmt(Stmt&) -> void;
      auto codegenExpr(Expr&) -> llvm::Value*;

      inline auto getCurrentStmt() const -> Stmt* {
        return mCurrentStmt;
      }
      inline auto setCurrentStmt(Stmt* s) -> void {
        mCurrentStmt = s;
      }

      inline auto getCurrentExpr() const -> Stmt* {
        return mCurrentStmt;
      }
      inline auto setCurrentExpr(Expr* e) -> void {
        mCurrentExpr = e;
      }

      inline auto setEvaluatedExpr(llvm::Value* value ) -> void { 
        mEvaluatedExpr = value; 
      };

      inline auto getEvaluatedExpr() const -> llvm::Value* { 
        return mEvaluatedExpr; 
      };

      auto visit(ExprStmt&) -> void final;
      auto visit(BlockStmt&) -> void final;
      auto visit(FunctionDeclaration&) -> void final;
      auto visit(VariableDeclaration&) -> void final;

      auto visit(LiteralExpr&) -> void final;
      auto visit(BinaryExpr&) -> void final;
      auto visit(UnaryExpr&) -> void final;

      auto convertType(ast::Type type) -> llvm::Type*;
  };

}
