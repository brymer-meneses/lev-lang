#pragma once
#include <memory>
#include "token.h"
#include <variant>
#include "visitor.h"
#include <vector>
#include <iostream>

namespace lev::ast {
  using lev::token::Token;

  enum class Type {
    UserDefined,

    i8,
    i16,
    i32,
    i64,

    u8,
    u16,
    u32,
    u64,

    f32,
    f64,
    
    Void,
  };

  constexpr auto typeToString(Type type) -> std::string_view {
    switch (type) {
      case Type::UserDefined:
        return "UserDefined";
      case Type::i8:
        return "i8";
      case Type::i16:
        return "i16";
      case Type::i32:
        return "i32";
      case Type::i64:
        return "i64";
      case Type::u8:
        return "u8";
      case Type::u16:
        return "u16";
      case Type::u32:
        return "u32";
      case Type::u64:
        return "u64";
      case Type::f32:
        return "f32";
      case Type::f64:
        return "f64";
      case Type::Void:
        return "void";
    }
  };

  struct Expr {
    virtual auto visit(ExprVisitor&) -> void = 0;
    virtual ~Expr() = default;

    virtual auto operator==(const Expr& e) const -> bool = 0;

    friend auto operator==(const std::unique_ptr<Expr>& e1, const std::unique_ptr<Expr>e2) -> bool {
      return *e1 == *e2;
    }

    template <typename T>
    inline auto as() -> T {
      return dynamic_cast<T>(this);
    }
  };

  struct LiteralExpr : Expr {
    Token token;
    LiteralExpr(Token token) : token(token) { }

    auto visit(ExprVisitor& v) -> void final {
      v.visit(*this);
    }

    auto operator==(const Expr& e) const -> bool final {
      if (const LiteralExpr* other = dynamic_cast<const LiteralExpr*>(&e)) {
        return token == other->token;
      }
      return false;
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

    auto operator==(const Expr& e) const -> bool final {
      if (const BinaryExpr* other = dynamic_cast<const BinaryExpr*>(&e)) {
        return *other->left == *left and other->op == op and *other->right == *right;
      }
      return false;
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

    auto operator==(const Expr& e) const -> bool {
      if (const UnaryExpr* other = dynamic_cast<const UnaryExpr*>(&e)) {
        return *other->left == *left and other->op == op;
      }
      return false;
    }
  };

  struct Stmt {
    virtual auto visit(StmtVisitor&) -> void = 0;
    virtual ~Stmt() = default;
    virtual auto operator==(const Stmt& e) const -> bool = 0;
    
    friend auto operator==(const std::unique_ptr<Stmt>& e1, const std::unique_ptr<Stmt>e2) -> bool {
      return *e1 == *e2;
    }

    template <typename T>
    inline auto as() -> T {
      return dynamic_cast<T>(this);
    }
  };

  struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;

    ExprStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {};

    auto visit(StmtVisitor& v) -> void final {
      v.visit(*this);
    }

    auto operator==(const Stmt& e) const -> bool {
      if (const ExprStmt* other = dynamic_cast<const ExprStmt*>(&e)) {
        return *other->expr == *expr;
      }
      return false;
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

    auto operator==(const Stmt& e) const -> bool final {
      if (const VariableDeclaration* other = dynamic_cast<const VariableDeclaration*>(&e)) {
        return other->identifier == identifier and 
               other->isMutable == isMutable and
               *other->value == *value and 
               other->type == type;
      }
      return false;
    }
  };

  using FunctionArg = std::tuple<std::string_view, Type>;

  struct FunctionDeclaration : Stmt {
    std::string_view functionName;
    std::vector<FunctionArg> args;
    Type returnType;
    std::vector<std::unique_ptr<Stmt>> body;

    FunctionDeclaration(std::string_view functionName,
                        std::vector<FunctionArg> args,
                        std::vector<std::unique_ptr<Stmt>> body,
                        Type returnType)
        : functionName(functionName), args(std::move(args)),
          returnType(returnType), body(std::move(body)) {}

    auto operator==(const Stmt& e) const -> bool final {
      if (const FunctionDeclaration* other = dynamic_cast<const FunctionDeclaration*>(&e)) {
        if (other->body.size() != body.size()) return false;

        for (int i = 0; i<body.size(); i++) {
          if (*body[i] != *other->body[i]) {
            return false;
          }
        }

        return other->functionName == functionName and 
               other->returnType == returnType and
               other->args == args;
      }
      return false;
    }

    auto visit(StmtVisitor& v) -> void final {
      v.visit(*this);
    }
  };
}
