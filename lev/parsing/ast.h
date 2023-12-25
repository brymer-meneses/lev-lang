#pragma once
#include <variant>
#include <memory>
#include <span>
#include <vector>
#include <optional>

#include <lev/parsing/token.h>
#include <lev/parsing/type.h>

namespace lev {

struct Expr {

  struct Binary {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    Token op;

    explicit Binary(Expr left, Expr right, Token op);
  };

  struct Unary {
    std::unique_ptr<Expr> right;
    Token op;

    explicit Unary(Expr right, Token op);
  };

  struct Literal {
    Token value;
    explicit Literal(Token value);
  };

  using ValueType = std::variant<Binary, Unary, Literal>;

  public:
    auto accept(auto visitor) const -> decltype(auto) {
      return std::visit(visitor, value);
    }

    template <typename T>
    requires std::is_constructible_v<ValueType, T>
    constexpr Expr(T value) : value(std::move(value)) {}

  private:
    ValueType value;

    friend constexpr auto operator==(const Expr& s1, const Expr& s2) -> bool;
};


struct FunctionArgument {
  Token identifier;
  LevType type;

  explicit FunctionArgument(Token identifier, LevType type);
};

struct Stmt;

struct Branch {
  Expr condition;
  std::unique_ptr<Stmt> body;

  Branch(Expr condition, Stmt stmt);
};

struct Stmt {

  struct VariableDeclaration {
    Token identifier;
    LevType type;
    Expr value;

    explicit VariableDeclaration(Token identifier, LevType type, Expr value);
  };

  struct FunctionDeclaration {
    Token identifier;
    std::vector<FunctionArgument> arguments;
    LevType returnType;
    std::unique_ptr<Stmt> body;

    explicit FunctionDeclaration(Token identifier, std::vector<FunctionArgument> arguments, LevType type, Stmt body);
  };

  struct Control {
    Branch ifBranch;
    std::vector<Branch> elseIfBranches;
    std::optional<std::unique_ptr<Stmt>> elseBody;

    Control(Branch ifBranch, std::vector<Branch> elseIfBranches, std::optional<Stmt> elseBody);
  };

  struct Assignment {
    Token identifier;
    Expr value;

    Assignment(Token identifier, Expr value);
  };

  struct Block {
    std::vector<Stmt> statements;
    explicit Block(std::vector<Stmt> statements);
  };

  struct Return {
    std::optional<Expr> expr = std::nullopt;
    explicit Return(Expr expr);
    explicit Return() {};
  };

  using ValueType = std::variant<VariableDeclaration, FunctionDeclaration, Block, Return, Control, Assignment>;

  public:
    constexpr auto accept(auto visitor) const -> decltype(auto) {
      return std::visit(visitor, value);
    };

    template <typename T>
    constexpr auto is() const -> bool {
      return std::holds_alternative<T>(value);
    }

    template <typename T>
    requires std::is_constructible_v<ValueType, T>
    constexpr Stmt(T value) : value(std::move(value)) {}

  private:
    ValueType value;

    friend constexpr auto operator==(const Stmt& s1, const Stmt& s2) -> bool;
};

};
