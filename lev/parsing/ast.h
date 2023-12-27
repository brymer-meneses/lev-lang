#pragma once
#include <variant>
#include <memory>
#include <span>
#include <vector>

#include <lev/parsing/token.h>
#include <lev/parsing/type.h>

namespace lev {

struct Stmt;
struct Expr;

struct Node {
  const Node* parent = nullptr;
  virtual ~Node() = default;

  constexpr auto setParent(Node* node) -> void {
    parent = node;
  }
};

struct Expr : Node {
  virtual auto operator==(const Expr& s1) const -> bool = 0;
};

struct Stmt : Node {
  virtual auto operator==(const Stmt& s1) const -> bool = 0;
};

struct Branch : Node {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> body;

  explicit Branch(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> stmt);

  auto operator==(const Branch&) const -> bool;
};

struct FunctionArgument : Node {
  Token identifier;
  LevType type;

  explicit FunctionArgument(Token identifier, LevType type);

  auto operator==(const FunctionArgument&) const -> bool;
};

struct BinaryExpr : Expr {
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  Token op;

  explicit BinaryExpr(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, Token op);

  auto operator==(const Expr&) const -> bool override final;
};

struct UnaryExpr : Expr {
  std::unique_ptr<Expr> right;
  Token op;

  explicit UnaryExpr(std::unique_ptr<Expr> right, Token op);

  auto operator==(const Expr&) const -> bool override final;
};

struct IdentifierExpr : Expr {
  Token identifier;
  explicit IdentifierExpr(Token identifier);

  auto operator==(const Expr&) const -> bool override final;
};

struct LiteralExpr : Expr {
  Token value;
  explicit LiteralExpr(Token value);

  auto operator==(const Expr&) const -> bool override final;
};


struct VariableDeclaration : Stmt {
  Token identifier;
  LevType type;
  std::unique_ptr<Expr> value;
  bool isMutable;

  explicit VariableDeclaration(Token identifier, LevType type, std::unique_ptr<Expr> value, bool isMutable);

  auto operator==(const Stmt&) const -> bool override final;
};

struct FunctionDeclaration : Stmt {
  Token identifier;
  std::vector<FunctionArgument> arguments;
  LevType returnType;
  std::unique_ptr<Stmt> body;

  explicit FunctionDeclaration(Token identifier, 
                               std::vector<FunctionArgument> arguments, 
                               LevType type, 
                               std::unique_ptr<Stmt> body);

  auto operator==(const Stmt&) const -> bool override final;
};

struct ControlStmt : Stmt {
  Branch ifBranch;
  std::vector<Branch> elseIfBranches;
  std::unique_ptr<Stmt> elseBody = nullptr;

  explicit ControlStmt(Branch ifBranch, std::unique_ptr<Stmt> elseBody, std::vector<Branch> elseIFBranches);

  template<typename... Args>
  explicit ControlStmt(Branch ifBranch, std::unique_ptr<Stmt> elseBody, Args&&... branches) 
    : ifBranch(std::move(ifBranch))
    , elseBody(std::move(elseBody)) { 
    (elseIfBranches.push_back(std::move(branches)), ...);
  }

  auto operator==(const Stmt&) const -> bool override final;
};

struct AssignmentStmt : Stmt {
  Token identifier;
  std::unique_ptr<Expr> value;

  AssignmentStmt(Token identifier, std::unique_ptr<Expr> value);

  auto operator==(const Stmt&) const -> bool override final;
};

struct BlockStmt : Stmt {
  std::vector<std::unique_ptr<Stmt>> statements;

  explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements);

  template<typename... Args>
  explicit BlockStmt(Args&&... args) {
      (statements.push_back(std::move(args)), ...);
  }

  auto operator==(const Stmt&) const -> bool override final;
};

struct ReturnStmt : Stmt {
  std::unique_ptr<Expr> expr = nullptr;
  explicit ReturnStmt(std::unique_ptr<Expr> expr = nullptr);

  auto operator==(const Stmt&) const -> bool override final;
};

};
