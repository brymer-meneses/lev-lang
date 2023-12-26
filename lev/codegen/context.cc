#include <lev/codegen/context.h>
#include <lev/misc/match.h>

#include <ranges>

using namespace lev;

auto Context::getCurrentScope() -> Scope& {
  return mScopes.back();
}

auto Context::createScope() -> Scope& {
  mScopes.push_back(Scope(mBuilder));
  return mScopes.back();
}

auto Context::popCurrentScope() -> void {
  mScopes.pop_back();
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

auto Context::getVariableInstruction(std::string_view name) const -> std::optional<llvm::AllocaInst*> {
  auto name_ = std::string(name);
  for (const auto& scope : std::ranges::views::reverse(mScopes)) {
    if (scope.isVariableDefined(name_)) {
      return scope.readVariable(name_);
    }
  }
  return std::nullopt;
}

auto Context::getVariableDeclaration(std::string_view name) const -> std::optional<const Stmt::VariableDeclaration*> {
  auto name_ = std::string(name);
  for (const auto& scope : std::ranges::views::reverse(mScopes)) {
    if (scope.isVariableDefined(name_)) {
      return scope.getVariableDeclaration(name_);
    }
  }
  return std::nullopt;
}

