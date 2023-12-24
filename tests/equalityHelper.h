#include <lev/parsing/ast.h>
#include <print>
#include <ranges>

namespace lev {

constexpr auto operator==(const Stmt& s1, const Stmt& s2) -> bool;
constexpr auto operator==(const Expr& e1, const Expr& e2) -> bool;

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

constexpr auto operator==(const FunctionArgument& t1, const FunctionArgument& t2) -> bool {
  return t1.identifier == t2.identifier and t1.type == t2.type;
}

constexpr auto operator==(const Branch& e1, const Branch& e2) -> bool {
  return e1.condition == e2.condition and *e1.body == *e2.body;
}

constexpr auto operator==(const Stmt::Block& s1, const Stmt::Block& s2) -> bool {
  for (const auto& [s1, s2] : std::views::zip(s1.statements, s2.statements)) {
    if (s1 != s2) {
      return false;
    }
  }
  return true;
}

constexpr auto operator==(const Stmt::FunctionDeclaration& s1, const Stmt::FunctionDeclaration& s2) -> bool {
  return s1.identifier == s2.identifier and *s1.body == *s2.body and s1.arguments == s2.arguments and s1.returnType == s2.returnType;
}
constexpr auto operator==(const Stmt::Return& s1, const Stmt::Return& s2) -> bool {
  return s1.expr == s2.expr;
}

constexpr auto operator==(const Stmt::VariableDeclaration& s1, const Stmt::VariableDeclaration& s2) -> bool {
  return s1.type == s2.type and s1.identifier == s2.identifier and s1.value == s1.value;
}

constexpr auto operator==(const Stmt::Control& e1, const Stmt::Control& e2) -> bool {
  if (e1.elseBody and not e2.elseBody) return false;
  if (not e1.elseBody and e2.elseBody) return false;

  if (e1.elseBody and e2.elseBody) {
    if (*e1.elseBody.value() != *e2.elseBody.value()) {
      std::println(stderr, "REACHED HERE!!");
      return false;
    }
  }

  return e1.ifBranch == e2.ifBranch and e1.elseIfBranches == e2.elseIfBranches;
}

constexpr auto operator==(const Expr::Binary& e1, const Expr::Binary& e2) -> bool {
  return e1.op == e2.op and *e1.left == *e2.left and *e1.right == *e2.right;
}

constexpr auto operator==(const Expr::Unary& e1, const Expr::Unary& e2) -> bool {
  return e1.op == e2.op and e1.right == e2.right;
}
constexpr auto operator==(const Expr::Literal& e1, const Expr::Literal& e2) -> bool {
  return e1.value == e2.value;
}

constexpr auto operator==(const Expr& e1, const Expr& e2) -> bool {
  return e1.value == e2.value;
}
constexpr auto operator==(const Stmt& e1, const Stmt& e2) -> bool {
  return e1.value == e2.value;
}

}

