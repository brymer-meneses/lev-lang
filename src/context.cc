#include "context.h"

#include <optional>
#include <assert.h>
#include <print>
#include <variant>

#include "token.h"
#include "utils.h"

using namespace lev::semantics;
using namespace lev::token;

static constexpr auto getDefaultType(TokenType type) -> std::optional<lev::ast::Type> {
  switch (type) {
    case TokenType::Integer:
      return lev::ast::Type::i32;
    case TokenType::Float:
      return lev::ast::Type::f32;
    case TokenType::True:
    case TokenType::False:
      return lev::ast::Type::Bool;
    case TokenType::String:
      return lev::ast::Type::String;
    default:
      return std::nullopt;
  }
} 

static constexpr auto getSize(Type type) -> size_t {
  switch (type) {
    case Type::i64:
    case Type::u64:
      return 8;
    case Type::i32:
    case Type::u32:
      return 4;
    case Type::i16:
    case Type::u16:
      return 2;
    case Type::i8:
    case Type::u8:
      return 1;
    default:
      return 0;
  }
}

auto SemanticContext::isSigned(Type type) -> bool {
  switch (type) {
    case Type::i64:
    case Type::i32:
    case Type::i16:
    case Type::i8:
    case Type::f64:
    case Type::f32:
      return true;
    default:
      return false;
  }
} 

auto SemanticContext::isInteger(Type type) -> bool {
  switch (type) {
    case Type::i64:
    case Type::i32:
    case Type::i16:
    case Type::i8:
    case Type::u64:
    case Type::u32:
    case Type::u16:
    case Type::u8:
      return true;
    default:
      return false;
  }
}

auto SemanticContext::isFloat(Type type) -> bool {
  switch (type) {
    case Type::f64:
    case Type::f32:
      return true;
    default:
      return false;
  }
}

auto SemanticContext::isNumber(Type type) -> bool {
  switch (type) {
    case Type::f64:
    case Type::f32:
    case Type::i64:
    case Type::i32:
    case Type::i16:
    case Type::i8:
    case Type::u64:
    case Type::u32:
    case Type::u16:
    case Type::u8:
      return true;
    default:
      return false;
  }
}


auto SemanticContext::isTriviallyCastableTo(const Type t1, const Type t2) -> bool {
  if (t1 != t2) return false;

  if (isNumber(t1) != isNumber(t2)) return false;

  if (isSigned(t1) != isSigned(t2)) return false;
  if (isInteger(t1) != isInteger(t2)) return false;

  if (getSize(t1) > getSize(t2)) {
    return false;
  }

  return true;
}

SemanticContext::SemanticContext() = default;

auto SemanticContext::reportError(const SemanticError& e) -> void {
  static auto visitor = overloaded {
    [](const InvalidUnaryType& e) {
      std::println(stderr, "Semantic Error: Invalid unary type");
    },
    [](const TypeMismatch& e) {
      std::println(stderr, "Semantic Error: Invalid unary type");
    },
    [](const Unimplemented& e) {
      std::println(stderr, "Semantic Error: Unimplemented \n{}", e.location.function_name());
    },
    [](const UndefinedVariable& e) {
      std::println(stderr, "Semantic Error: Undefined Variable");
    },
    [](const NoInformation& e) {
      std::println(stderr, "Semantic Error: No information??");
    }
  };

  std::visit(visitor, e);
}

auto SemanticContext::setCurrentStmt(const Stmt* stmt) -> void {
  assert(stmt != nullptr);

  static const auto visitor = overloaded {
    [this](const VariableDeclaration& v) {
      mFunctionStack[std::string(v.identifier.lexeme)] = v.type;
    },
    [](const auto&) {}
  };

  stmt->accept(visitor);
  mCurrentStmt = stmt;
}

auto SemanticContext::getVariableType(std::string_view name) const -> std::expected<ast::Type, SemanticError> {
  const auto key = std::string(name);
  if (mFunctionStack.contains(key)) {
    return mFunctionStack.at(key);
  }
  return std::unexpected(UndefinedVariable{});
}

auto SemanticContext::inferTypeFromCurrentStmt() const -> std::expected<ast::Type, SemanticError> {
  assert(mCurrentStmt != nullptr);
  return inferStmtType(*mCurrentStmt);
}

auto SemanticContext::inferType(const CallExpr& e) const -> std::expected<ast::Type, SemanticError> {
  return std::unexpected(Unimplemented{});
}

auto SemanticContext::inferType(const LiteralExpr& e) const -> std::expected<ast::Type, SemanticError> {
  assert(mCurrentStmt != nullptr);

  if (mCurrentStmt->holds<VariableDeclaration>()) {
    return mCurrentStmt->get<VariableDeclaration>().type;
  }

  auto type = getDefaultType(e.value.type);
  if (not type) {
    return std::unexpected(NoInformation{});
  }
  return *type;
}

auto SemanticContext::inferType(const VariableExpr& e) const -> std::expected<ast::Type, SemanticError> {
  auto type = getVariableType(e.identifier.lexeme);
  return type;
}

auto SemanticContext::inferType(const UnaryExpr& e) const -> std::expected<ast::Type, SemanticError> {
  if (e.op.type == TokenType::Bang) {
    if (inferExprType(*e.right) == Type::Bool) {
      return Type::Bool;
    } else {
      return std::unexpected(InvalidUnaryType{});
    }
  }
  return std::unexpected(Unimplemented{});
}

auto SemanticContext::inferType(const BinaryExpr& e) const -> std::expected<ast::Type, SemanticError> {

  const auto leftType = inferExprType(*e.left);
  const auto rightType = inferExprType(*e.right);

  if (not leftType) {
    return rightType;
  }
  if (not rightType) {
    return rightType;
  }

  if (leftType == Type::i32 and rightType == Type::i32) {
    return Type::i32;
  }
  if (leftType == Type::f32 and rightType == Type::f32) {
    return Type::f32;
  }
  if (leftType == Type::i32 and rightType == Type::f32) {
    return Type::f32;
  }
  if (leftType == Type::f32 and rightType == Type::i32) {
    return Type::f32;
  }
  return std::unexpected(Unimplemented{});
}

auto SemanticContext::inferExprType(const Expr& e) const -> std::expected<ast::Type, SemanticError> {
  return e.accept([this](const auto& e){ return inferType(e); });
}

auto SemanticContext::inferStmtType(const Stmt& s) const -> std::expected<ast::Type, SemanticError> {
  static const auto visitor = overloaded {
    [](const VariableDeclaration& e) -> std::expected<ast::Type, SemanticError> {
      return e.type;
    },
    [](const FunctionDeclaration& e) -> std::expected<ast::Type, SemanticError> {
      return e.returnType;
    },
    [](const auto& e) -> std::expected<ast::Type, SemanticError> {
      return std::unexpected(Unimplemented{});
    }
  };
  return s.accept(visitor);
}
