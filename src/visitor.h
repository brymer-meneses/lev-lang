#pragma once

namespace lev::ast {
  struct LiteralExpr;
  struct BinaryExpr;
  struct UnaryExpr;

  struct ExprStmt;
  struct BlockStmt;
  struct VariableDeclaration;
  struct FunctionDeclaration;

  struct ExprVisitor {
    virtual auto visit(LiteralExpr& expr) -> void = 0;
    virtual auto visit(BinaryExpr& expr) -> void = 0;
    virtual auto visit(UnaryExpr& expr) -> void = 0;
  };

  struct StmtVisitor {
    virtual auto visit(ExprStmt& expr) -> void = 0;
    virtual auto visit(BlockStmt& expr) -> void = 0;
    virtual auto visit(VariableDeclaration& expr) -> void = 0;
    virtual auto visit(FunctionDeclaration& expr) -> void = 0;
  };
  

}
