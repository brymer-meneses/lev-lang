#pragma once

namespace lev::ast {
  struct LiteralExpr;
  struct BinaryExpr;
  struct UnaryExpr;

  struct ExprStmt;
  struct VariableDeclaration;

  struct ExprVisitor {
    virtual auto visit(LiteralExpr& expr) -> void = 0;
    virtual auto visit(BinaryExpr& expr) -> void = 0;
    virtual auto visit(UnaryExpr& expr) -> void = 0;
  };

  struct StmtVisitor {
    virtual auto visit(ExprStmt& expr) -> void = 0;
    virtual auto visit(VariableDeclaration& expr) -> void = 0;
  };
  

}
