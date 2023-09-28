#pragma once

#include "ast.h"
#include <expected>
#include <source_location>
#include <map>


namespace lev::semantics {
  using namespace lev::ast;

  struct InvalidUnaryType {
    std::source_location location;
    InvalidUnaryType(const std::source_location loc = std::source_location::current()) 
      : location(loc) { }
  };

  struct TypeMismatch {
    std::source_location location;
    TypeMismatch(const std::source_location loc = std::source_location::current()) 
      : location(loc) { }
  };

  struct Unimplemented {
    std::source_location location;
    Unimplemented(const std::source_location loc = std::source_location::current()) 
      : location(loc) { }
  };
  struct UndefinedVariable {
    std::source_location location;
    UndefinedVariable(const std::source_location loc = std::source_location::current()) 
      : location(loc) { }
  };
  struct NoInformation {
    std::source_location location;
    NoInformation(const std::source_location loc = std::source_location::current()) 
      : location(loc) { }
  };

  using SemanticError = std::variant<InvalidUnaryType, UndefinedVariable, Unimplemented, TypeMismatch, NoInformation>;

  class SemanticContext {
    public:
      SemanticContext(std::vector<Stmt> statements) : statements(std::move(statements)) {}
      SemanticContext();

      auto inferExprType(const Expr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferStmtType(const Stmt&) const -> std::expected<ast::Type, SemanticError>;

      auto inferTypeFromCurrentStmt() const -> std::expected<ast::Type, SemanticError>;
      auto setCurrentStmt(const Stmt*) -> void;

      static auto isInteger(const Type) -> bool;
      static auto isFloat(const Type) -> bool;
      static auto isSigned(const Type) -> bool;
      static auto isNumber(const Type) -> bool;
      static auto reportError(const SemanticError&) -> void;

      auto inferType(const UnaryExpr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const BinaryExpr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const CallExpr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const VariableExpr&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const LiteralExpr&) const -> std::expected<ast::Type, SemanticError>;

      auto inferType(const ExprStmt&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const AssignStmt&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const BlockStmt&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const FunctionDeclaration&) const -> std::expected<ast::Type, SemanticError>;
      auto inferType(const VariableDeclaration&) const -> std::expected<ast::Type, SemanticError>;

      auto getVariableType(std::string_view) const -> std::expected<ast::Type, SemanticError>;

      static auto isTriviallyCastableTo(const Type, const Type) -> bool;

    public:
      std::vector<Stmt> statements;

    private:
      const Stmt* mCurrentStmt = nullptr;
      std::map<std::string, ast::Type> mFunctionStack;
  };
}
