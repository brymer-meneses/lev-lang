#include <lev/codegen/context.h>
#include <lev/misc/match.h>

#include <ranges>
#include <print>

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
    [this](const Stmt::Return& s) -> std::optional<LevType> {
      auto statement = getFirstStatementWithType<Stmt::FunctionDeclaration>();
      if (not statement) {
        return std::nullopt;
      }
      return statement.value()->returnType;
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

auto Context::typeIsFloat(const LevType& type) -> bool {
  if (not type.is<LevType::Builtin>()) {
    return false;
  }

  auto typeValue = type.as<LevType::Builtin>();

  switch (typeValue.type) {
    case LevType::Builtin::Types::f32:
    case LevType::Builtin::Types::f64:
      return true;
    default:
      return false;
  }
}

auto Context::typeIsInteger(const LevType& type) -> bool {
  if (not type.is<LevType::Builtin>()) {
    return false;
  }

  auto typeValue = type.as<LevType::Builtin>();

  switch (typeValue.type) {
    case LevType::Builtin::Types::i8:
    case LevType::Builtin::Types::i16:
    case LevType::Builtin::Types::i32:
    case LevType::Builtin::Types::i64:
    case LevType::Builtin::Types::u8:
    case LevType::Builtin::Types::u16:
    case LevType::Builtin::Types::u32:
    case LevType::Builtin::Types::u64:
      return true;
    default:
      return false;
  }
}


auto Context::typeIsSigned(const LevType& type) -> bool {
  if (not type.is<LevType::Builtin>()) {
    return false;
  }

  auto typeValue = type.as<LevType::Builtin>();

  switch (typeValue.type) {
    case LevType::Builtin::Types::i8:
    case LevType::Builtin::Types::i16:
    case LevType::Builtin::Types::i32:
    case LevType::Builtin::Types::i64:
      return true;
    default:
      return false;
  }
}

