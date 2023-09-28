#pragma once

#include <memory>
#include <variant>
#include <vector>
#include <iostream>
#include <optional>
#include <concepts>

#include "token.h"

namespace lev::ast {
  using lev::token::Token;

  enum class Type {
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

    UserDefined,
    Bool,
    String,
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
      case Type::Bool:
        return "bool";
      case Type::String:
        return "string";
    }
  };

  class Expr;

  struct BinaryExpr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    Token op;

    BinaryExpr(Token op, Expr left, Expr right);
    friend auto operator==(const BinaryExpr& e1, const BinaryExpr& e2) -> bool;
  };

  struct UnaryExpr {
    std::unique_ptr<Expr> right;
    Token op;

    UnaryExpr(Token op, Expr right);
    friend auto operator==(const UnaryExpr& e1, const UnaryExpr& e2) -> bool;
  };

  struct VariableExpr {
    Token identifier;

    VariableExpr(Token identifier);
    friend auto operator==(const VariableExpr& e1, const VariableExpr& e2) -> bool;
  };

  struct LiteralExpr {
    Token value;

    LiteralExpr(Token value);
    friend auto operator==(const LiteralExpr& e1, const LiteralExpr& e2) -> bool;
  };

  struct CallExpr {
    using Arg = std::pair<std::string_view, std::unique_ptr<Expr>>; 

    Token identifier;
    std::vector<Arg> args;

    CallExpr(Token identifier, std::vector<Arg> args);
    friend auto operator==(const CallExpr& e1, const CallExpr& e2) -> bool;
  };

  class Expr {
    public:
      using Data = std::variant<BinaryExpr, UnaryExpr, VariableExpr, LiteralExpr, CallExpr>;

      Expr(BinaryExpr expr) : mData(std::move(expr)) {}
      Expr(UnaryExpr expr) : mData(std::move(expr)) {}
      Expr(VariableExpr expr) : mData(std::move(expr)) {}
      Expr(LiteralExpr expr) : mData(std::move(expr)) {}
      Expr(CallExpr expr) : mData(std::move(expr)) {}

    private:
      Data mData;

    public:
      friend auto operator==(const Expr& e1, const Expr& e2) -> bool;

      auto accept(auto visitor) const -> decltype(std::visit(visitor, mData)) {
        return std::visit(visitor, mData);
      }
  };

  class Stmt;

  struct VariableDeclaration {
    Token identifier;
    bool isMutable;
    std::unique_ptr<Expr> value;
    Type type;

    VariableDeclaration(Token identifier, bool isMutable, Expr value, Type type);

    friend auto operator==(const VariableDeclaration& e1, const VariableDeclaration& e2) -> bool;
  };

  using FunctionArg = std::pair<std::string_view, Type>;
  struct FunctionDeclaration {
    std::string_view functionName;
    std::vector<FunctionArg> args;
    Type returnType;
    std::unique_ptr<Stmt> body;

    FunctionDeclaration(std::string_view functionName, std::vector<FunctionArg> args, Type returnType, Stmt body);
    friend auto operator==(const FunctionDeclaration& e1, const FunctionDeclaration& e2) -> bool;
  };

  struct ExprStmt {
    std::unique_ptr<Expr> expr;

    ExprStmt(Expr expr);
    friend auto operator==(const ExprStmt& e1, const ExprStmt& e2) -> bool;
  };


  struct IfStmt {
    struct Branch {
      Expr condition;
      std::unique_ptr<Stmt> body;

      Branch(Expr condition, Stmt stmt);
      friend auto operator==(const Branch& e1, const Branch& e2) -> bool;
    };

    Branch ifBranch;
    std::vector<Branch> elseIfBranches;
    std::optional<std::unique_ptr<Stmt>> elseBody;

    IfStmt(Branch ifBranch, std::vector<Branch> elseIfBranches, std::optional<Stmt> elseBody);

    friend auto operator==(const IfStmt& e1, const IfStmt& e2) -> bool;
  };

  struct AssignStmt {
    Token identifier;
    std::unique_ptr<Expr> value;

    AssignStmt(Token identifier, Expr value);
    friend auto operator==(const AssignStmt& e1, const AssignStmt& e2) -> bool;
  };
  
  struct BlockStmt {
    std::vector<Stmt> statements;

    BlockStmt(std::vector<Stmt> statements);
    explicit BlockStmt(Stmt statement);

    friend auto operator==(const BlockStmt& e1, const BlockStmt& e2) -> bool;
  };

  struct ReturnStmt {
    std::optional<Expr> expr;
    ReturnStmt(std::optional<Expr> expr);

    friend auto operator==(const ReturnStmt& e1, const ReturnStmt& e2) -> bool;
  };

  class Stmt {
    private:
      using Data = std::variant<VariableDeclaration, FunctionDeclaration, ExprStmt, AssignStmt, BlockStmt, IfStmt, ReturnStmt>;
      Data mData;

    public:
      Stmt(VariableDeclaration data) : mData(std::move(data)) {}
      Stmt(FunctionDeclaration data) : mData(std::move(data)) {}
      Stmt(ExprStmt data) : mData(std::move(data)) {}
      Stmt(AssignStmt data) : mData(std::move(data)) {}
      Stmt(BlockStmt data) : mData(std::move(data)) {}
      Stmt(IfStmt data) : mData(std::move(data)) {}
      Stmt(ReturnStmt data) : mData(std::move(data)) {}

      auto accept(const auto visitor) const -> decltype(std::visit(visitor, mData)) {
        return std::visit(visitor, mData);
      }

      template <typename T>
      auto holds() const -> bool {
        return std::holds_alternative<T>(mData);
      }

      template <typename T>
      auto get() const -> const T& {
        return std::get<T>(mData);
      }


      friend auto operator==(const Stmt& e1, const Stmt& e2) -> bool;
  };
}
