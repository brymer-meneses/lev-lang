#pragma once

#include "ast.h"
#include <expected>
#include <map>


namespace lev::semantics {
  using namespace lev::ast;

  struct InvalidUnaryType {};
  struct TypeMismatch {};
  struct Unimplemented {};
  struct UndefinedVariable {};
  struct NoInformation {};

  using SemanticError = std::variant<InvalidUnaryType, UndefinedVariable, Unimplemented, TypeMismatch, NoInformation>;

  class SemanticContext {
    public:
      SemanticContext(std::vector<Stmt> statements) : statements(std::move(statements)) {}
      SemanticContext() = default;

      auto inferExprType(const Expr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferStmtType(const Stmt&) const -> std::expected<ast::Type, SemanticError>;

      auto inferType(const Expr::UnaryExpr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const Expr::BinaryExpr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const Expr::CallExpr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const Expr::VariableExpr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const Expr::LiteralExpr&) const -> std::expected<ast::Type, SemanticError>;

      auto inferType(const Stmt::ExprStmt&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const Stmt::AssignStmt&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const Stmt::BlockStmt&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const Stmt::FunctionDeclarationStmt&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const Stmt::VariableDeclarationStmt&) const -> std::expected<ast::Type, SemanticError>;

      auto getVariableContext(std::string_view) const -> std::expected<const Stmt*, SemanticError>;

      auto setCurrentStmt(const Stmt* s) -> void;

      static auto isTriviallyCastableTo(const Type, const Type) -> bool;

    public:
      std::vector<Stmt> statements;

    private:
      const Stmt* mCurrentStmt;
  };
}
