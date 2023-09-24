#pragma once

namespace lev::ast {
  struct LiteralExpr;
  struct BinaryExpr;
  struct UnaryExpr;

  struct ExprStmt;
  struct BlockStmt;
  struct AssignStmt;
  struct VariableDeclaration;
  struct FunctionDeclaration;

  struct ExprVisitor {
    virtual auto visit(LiteralExpr&) -> void = 0;
    virtual auto visit(BinaryExpr&) -> void = 0;
    virtual auto visit(UnaryExpr&) -> void = 0;
  };

  struct StmtVisitor {
    virtual auto visit(ExprStmt&) -> void = 0;
    virtual auto visit(BlockStmt&) -> void = 0;
    virtual auto visit(VariableDeclaration&) -> void = 0;
    virtual auto visit(FunctionDeclaration&) -> void = 0;
    virtual auto visit(AssignStmt&) -> void = 0;
  };
  

}
