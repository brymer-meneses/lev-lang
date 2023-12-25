#include <lev/codegen/context.h>
#include <lev/misc/match.h>

using namespace lev;

auto Context::getCurrentScope() -> Scope& {
  return mScopes.top();
}

auto Context::createScope() -> Scope& {
  mScopes.push(Scope(mBuilder));
  return mScopes.top();
}

auto Context::popCurrentScope() -> void {
  mScopes.pop();
}

auto Context::getCurrentStatement() -> const Stmt* {
  return getCurrentScope().statements.back();
}

auto Context::getAppropriateExprType() -> std::optional<LevType> {
  auto* statement = getCurrentStatement();

  return statement->accept(match {
    [](const Stmt::VariableDeclaration& s) -> std::optional<LevType> {
      return s.type;
    },
    [](const auto& s) -> std::optional<LevType> {
      return std::nullopt;
    },
  });
}
