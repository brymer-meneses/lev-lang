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
    virtual auto accept(ExprVisitor&) -> void = 0;
    virtual ~Expr() = default;

    virtual auto operator==(const Expr& e) const -> bool = 0;

    friend auto operator==(const std::unique_ptr<Expr>& e1, const std::unique_ptr<Expr>e2) -> bool {
      return *e1 == *e2;
    }

    template <typename T>
    inline auto as() -> T {
      return static_cast<T>(this);
    }
  };

  struct LiteralExpr : Expr {
    Token token;
    LiteralExpr(Token token) : token(token) { }

    auto accept(ExprVisitor& v) -> void final {
      v.visit(*this);
    }

    auto operator==(const Expr& e) const -> bool final {
      if (const LiteralExpr* other = static_cast<const LiteralExpr*>(&e)) {
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

    auto accept(ExprVisitor& v) -> void final {
      v.visit(*this);
    }

    auto operator==(const Expr& e) const -> bool final {
      if (const BinaryExpr* other = static_cast<const BinaryExpr*>(&e)) {
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

    auto accept(ExprVisitor& v) -> void final {
      v.visit(*this);
    }

    auto operator==(const Expr& e) const -> bool final {
      if (const UnaryExpr* other = static_cast<const UnaryExpr*>(&e)) {
        return *other->left == *left and other->op == op;
      }
      return false;
    }
  };

  struct Stmt {
    virtual auto accept(StmtVisitor&) -> void = 0;
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

    auto accept(StmtVisitor& v) -> void final {
      v.visit(*this);
    }

    auto operator==(const Stmt& e) const -> bool final {
      if (const ExprStmt* other = static_cast<const ExprStmt*>(&e)) {
        return *other->expr == *expr;
      }
      return false;
    }
  };

  struct AssignStmt : Stmt {
    std::unique_ptr<Expr> identifier;
    std::unique_ptr<Expr> value;

    AssignStmt(std::unique_ptr<Expr> identifier, std::unique_ptr<Expr> value) 
      : identifier(std::move(identifier)), value(std::move(value)) {}

    auto accept(StmtVisitor& v) -> void final {
      v.visit(*this);
    }

    auto operator==(const Stmt& e) const -> bool final {
      if (const AssignStmt* other = static_cast<const AssignStmt*>(&e)) {
        return *other->identifier == *identifier and *other->value == *value;
      }
      return false;
    }

  };

  struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt() = default;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}
    
    auto accept(StmtVisitor& v) -> void final {
      v.visit(*this);
    }

    auto addStmt(std::unique_ptr<Stmt> stmt) -> void {
      statements.push_back(std::move(stmt));
    }
    auto operator==(const Stmt& e) const -> bool final {
      if (const BlockStmt* other = static_cast<const BlockStmt*>(&e)) {
        if (other->statements.size() != statements.size()) {
          return false;
        }
        for (int i=0; i < statements.size(); i++) {
          if (*other->statements[i] != *statements[i]) {
            return false;
          }
        }
      }
      return true;
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
    auto accept(StmtVisitor& v) -> void final {
      v.visit(*this);
    }

    auto operator==(const Stmt& e) const -> bool final {
      if (const VariableDeclaration* other = static_cast<const VariableDeclaration*>(&e)) {
        return other->identifier == identifier and 
               other->isMutable == isMutable and
               *other->value == *value and 
               other->type == type;
      }
      return false;
    }
  };

  using FunctionArg = std::pair<std::string_view, Type>;

  struct FunctionDeclaration : Stmt {
    std::string_view functionName;
    std::vector<FunctionArg> args;
    Type returnType;
    std::unique_ptr<Stmt> body;

    FunctionDeclaration(std::string_view functionName,
                        std::vector<FunctionArg> args,
                        Type returnType,
                        std::unique_ptr<Stmt> body)
        : functionName(functionName), args(args),
          returnType(returnType), body(std::move(body)) {}

    auto operator==(const Stmt& e) const -> bool final {
      if (const FunctionDeclaration* other = static_cast<const FunctionDeclaration*>(&e)) {
        return other->functionName == functionName and 
               other->returnType == returnType and
               other->args == args and 
               *other->body == *body;
      }
      return false;
    }

    auto accept(StmtVisitor& v) -> void final {
      v.visit(*this);
    }
  };
}
