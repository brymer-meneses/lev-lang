#pragma once
#include <variant>
#include <memory>
#include <vector>

#include <lev/parsing/token.h>
#include <lev/parsing/type.h>

namespace lev {

struct Expr;
struct Stmt;

struct BinaryExpr;
struct UnaryExpr;

struct BinaryExpr {
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  Token op;

  explicit BinaryExpr(Expr left, Expr right, Token op);
};

struct UnaryExpr {
  std::unique_ptr<Expr> right;
  Token op;

  explicit UnaryExpr(Expr right, Token op);
};

struct LiteralExpr {
  Token value;
  explicit LiteralExpr(Token value);
};

struct Expr {
  using ValueType = std::variant<BinaryExpr, UnaryExpr, LiteralExpr>;

  public:
    auto accept(auto visitor) -> decltype(auto) {
      return std::visit(visitor, value);
    }

    template <typename T>
    requires std::is_constructible_v<ValueType, T>
    Expr(T value) : value(value) {}

  private:
    ValueType value;

    friend constexpr auto operator==(const Expr& s1, const Expr& s2) -> bool;
};

struct VariableDeclaration {
  Token identifier;
  LevType type;
  Expr value;

  explicit VariableDeclaration(Token identifier, LevType type, Expr value);
};

struct FunctionArgument {
  Token identifier;
  LevType type;

  explicit FunctionArgument(Token identifier, LevType type);
};

struct FunctionDeclaration {
  Token identifier;
  std::vector<FunctionArgument> arguments;
  LevType returnType;

  explicit FunctionDeclaration(Token identifier, std::vector<FunctionArgument> arguments, LevType type);
};

struct BlockStmt {
  std::vector<Stmt> statements;
  explicit BlockStmt(std::vector<Stmt> statements);
};

struct Stmt {
  using ValueType = std::variant<VariableDeclaration, FunctionDeclaration, BlockStmt>;

  public:
    auto accept(auto visitor) -> decltype(auto) {
      return std::visit(visitor, value);
    };

    template <typename T>
    requires std::is_constructible_v<ValueType, T>
    Stmt(T value) : value(std::move(value)) {}

  private:
    ValueType value;

    friend constexpr auto operator==(const Stmt& s1, const Stmt& s2) -> bool;
};

};
