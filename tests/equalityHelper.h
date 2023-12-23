#include <lev/parsing/ast.h>
#include <print>
#include <ranges>

namespace lev {

constexpr auto operator==(const Stmt& s1, const Stmt& s2) -> bool {
  return true;
}
constexpr auto operator==(const Expr& e1, const Expr& e2) -> bool {
  return true;
}

constexpr auto operator==(const Token& t1, const Token& t2) -> bool {
  return t1.type == t2.type and t1.lexeme == t2.lexeme;
}

constexpr auto operator==(const LevType::Builtin& t1, const LevType::Builtin& t2) -> bool {
  return t1.type == t2.type;
}
constexpr auto operator==(const LevType::Generic& t1, const LevType::Generic& t2) -> bool {
  // FIXME: true for now
  return true;
}
constexpr auto operator==(const LevType::Inferred& t1, const LevType::Inferred& t2) -> bool {
  // FIXME: true for now
  return true;
}

constexpr auto operator==(const LevType& t1, const LevType& t2) -> bool {
  return t1.value == t2.value;
}

constexpr auto operator==(const BlockStmt& s1, const BlockStmt& s2) -> bool {
  for (const auto& [s1, s2] : std::views::zip(s1.statements, s2.statements)) {
    if (s1 != s2) {
      return false;
    }
  }
  return true;
}

constexpr auto operator==(const FunctionDeclarationStmt& s1, const FunctionDeclarationStmt& s2) -> bool {
  return true;
}

constexpr auto operator==(const VariableDeclarationStmt& s1, const VariableDeclarationStmt& s2) -> bool {
  return s1.type == s2.type and s1.identifier == s2.identifier and s1.value == s1.value;
}

constexpr auto operator==(const BinaryExpr& e1, const BinaryExpr& e2) -> bool {
  return e1.op == e2.op and e1.left == e2.left and e1.right == e2.right;
}

constexpr auto operator==(const UnaryExpr& e1, const UnaryExpr& e2) -> bool {
  return e1.op == e2.op and e1.right == e2.right;
}
constexpr auto operator==(const LiteralExpr& e1, const LiteralExpr& e2) -> bool {
  return e1.value == e2.value;
}

}

