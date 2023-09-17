#pragma once
#include <memory>
#include "token.h"
#include <variant>
#include "visitor.h"

namespace lev::ast {
  using namespace lev::token;

  struct Expr {
    virtual auto visit(ExprVisitor&) -> void = 0;
    virtual ~Expr() = default;
  };

  struct LiteralExpr : Expr {
    Token token;
    LiteralExpr(Token token) : token(token) { }

    auto visit(ExprVisitor& v) -> void final {
      v.visit(*this);
    }
  };

  struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, 
               Token op,
               std::unique_ptr<Expr> right)
        : left(std::move(left))
        , op(op) 
        , right(std::move(right)) {};

    auto visit(ExprVisitor& v) -> void final {
      v.visit(*this);
    }
  };

  struct UnaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;

    UnaryExpr(std::unique_ptr<Expr> left, Token op)
        : left(std::move(left))
        , op(op) {}

    auto visit(ExprVisitor& v) -> void final {
      v.visit(*this);
    }
  };

  struct Stmt {
    virtual auto visit(StmtVisitor&) -> void = 0;
    virtual ~Stmt() = default;
  };

  struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;

    ExprStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {};

    auto visit(StmtVisitor& v) -> void final {
      v.visit(*this);
    }
  };

  struct VariableDeclaration : Stmt {
    Token identifier;
    bool isMutable;
    std::unique_ptr<Expr> value;
    Type type;


    VariableDeclaration(Token identifier, 
                        bool isMutable,
                        Type type,
                        std::unique_ptr<Expr> value)
                      : identifier(identifier)
                      , isMutable(isMutable)
                      , value(std::move(value)) 
                      , type(type) {}
    auto visit(StmtVisitor& v) -> void final {
      v.visit(*this);
    }
  };
}
