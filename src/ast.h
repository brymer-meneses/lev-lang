#pragma once

#include <memory>
#include "token.h"
#include <variant>
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

  class Expr {

    public:
      struct BinaryExpr {
        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;
        Token op;
        auto operator==(const BinaryExpr& o) const -> bool {
          return *left == *o.left and *right == *o.right and op == o.op;
        }
      };

      struct UnaryExpr {
        std::unique_ptr<Expr> right;
        Token op;
        auto operator==(const UnaryExpr& o) const -> bool {
          return *right == *o.right and op == o.op;
        }
      };

      struct VariableExpr {
        Token identifier;
        auto operator==(const VariableExpr& o) const -> bool {
          return identifier == o.identifier;
        }
      };

      struct LiteralExpr {
        Token value;
        auto operator==(const LiteralExpr& o) const -> bool {
          return value == o.value;
        }
      };
    
      using Data = std::variant<BinaryExpr, UnaryExpr, VariableExpr, LiteralExpr>;

    private:
      Data mData;
      Expr(Data data) : mData(std::move(data)) {}
      Expr() = default;
      

    public:
      auto operator==(const Expr&) const -> bool = default;

      auto accept(auto visitor) const -> decltype(std::visit(visitor, mData)) {
        return std::visit(visitor, mData);
      }

      static auto Binary(Token op, Expr left, Expr right) -> Expr {
        BinaryExpr expr;
        expr.op = op;
        expr.left = std::make_unique<Expr>(std::move(left));
        expr.right = std::make_unique<Expr>(std::move(right));
        return Expr(std::move(expr));
      }

      static auto Unary(Token op, Expr value) -> Expr {
        UnaryExpr expr;
        expr.op = op;
        expr.right = std::make_unique<Expr>(std::move(value));
        return Expr(std::move(expr));
      }
  
      static auto Variable(Token identifier) -> Expr {
        VariableExpr expr;
        expr.identifier = identifier;
        return Expr(std::move(expr));
      }

      static auto Literal(Token token) -> Expr {
        LiteralExpr expr;
        expr.value = token;
        return Expr(expr);
      }

  };

  using FunctionArg = std::pair<std::string_view, Type>;

  class Stmt {
    public:
      struct VariableDeclarationStmt {
        Token identifier;
        bool isMutable;
        std::unique_ptr<Expr> value;
        Type type;
        auto operator==(const VariableDeclarationStmt& o) const -> bool {
          return identifier == o.identifier and 
                 isMutable == o.isMutable and
                 *value == *o.value and
                 type == o.type;
        }
      };
      struct FunctionDeclarationStmt {
        std::string_view functionName;
        std::vector<FunctionArg> args;
        Type returnType;
        std::unique_ptr<Stmt> body;

        auto operator==(const FunctionDeclarationStmt& o) const -> bool {
          return functionName == o.functionName and
                 args == o.args and
                 returnType == o.returnType and
                 *body == *o.body;
        }
      };

      struct ExprStmt {
        std::unique_ptr<Expr> expr;
        auto operator==(const ExprStmt& o) const -> bool {
          return *expr == *o.expr;
        }
      };

      struct AssignStmt {
        Token identifier;
        std::unique_ptr<Expr> value;
        auto operator==(const AssignStmt& o) const -> bool {
          return identifier == o.identifier and *value == *o.value;
        }
      };
      
      struct BlockStmt {
        std::vector<Stmt> statements;
        auto operator==(const BlockStmt& o) const -> bool {
          if (statements.size() != statements.size()) {
            return false;
          }
          for (auto i=0; i<statements.size(); i++) {
            if (statements[i] != o.statements[i]) {
              return false;
            };
          }
          return true;
        }
      };

      using Data = std::variant<VariableDeclarationStmt, FunctionDeclarationStmt,
                                ExprStmt, AssignStmt, BlockStmt>;
      Data mData;

      Stmt(Data data) : mData(std::move(data)) {}

    public:
      auto operator==(const Stmt&) const -> bool = default;

      auto accept(const auto visitor) const -> decltype(std::visit(visitor, mData)) {
        return std::visit(visitor, mData);
      }

      static auto VariableDeclaration(Token identifier, bool isMutable, Expr value, Type type) -> Stmt {
        VariableDeclarationStmt v;
        v.identifier = identifier;
        v.isMutable = isMutable;
        v.value = std::make_unique<Expr>(std::move(value));
        v.type = type;
        return Stmt(std::move(v));
      }

      static auto FunctionDeclaration(std::string_view functionName, std::vector<FunctionArg> args, Type returnType, Stmt body) {
        FunctionDeclarationStmt v;
        v.args = std::move(args);
        v.functionName = functionName;
        v.returnType = returnType;
        v.body = std::make_unique<Stmt>(std::move(body));
        return Stmt(std::move(v));
      }

      static auto Assign(Token identifier, Expr value) -> Stmt {
        struct AssignStmt v;
        v.identifier = identifier;
        v.value = std::make_unique<Expr>(std::move(value));
        return Stmt(std::move(v));
      }

      static auto Block(std::vector<Stmt> statements) -> Stmt {
        struct BlockStmt v;
        v.statements = std::move(statements);
        return Stmt(std::move(v));
      }

      static auto Expression(Expr expr) -> Stmt {
        struct ExprStmt v;
        v.expr = std::make_unique<Expr>(std::move(expr));
        return Stmt(std::move(v));
      }
  };

    

}
