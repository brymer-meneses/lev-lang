#pragma once
#include "codegen/scope.h"

#include <ranges>
#include <vector>

namespace lev {

class Context {
private:
  
  std::vector<Scope> mScopes;
  std::shared_ptr<llvm::IRBuilder<>> mBuilder;

public:
  Context(std::shared_ptr<llvm::IRBuilder<>> builder) : mBuilder(builder) {}
  Context() = default;

  auto createScope() -> Scope&;
  auto getCurrentScope() -> Scope&;
  auto popCurrentScope() -> void;

  auto getCurrentStatement() -> const Stmt*;

  /// Iterates through the current scope to the root scope and gets the `llvm::AllocaInst` for that variable
  auto getVariableInstruction(std::string_view) const -> std::optional<llvm::AllocaInst*>;

  /// Iterates through the current scope to the root scope and gets the `Stmt::VariableDeclaration` for that variable
  auto getVariableDeclaration(std::string_view) const -> std::optional<const Stmt::VariableDeclaration*>;

  auto getAppropriateExprType() -> std::optional<LevType>;

  template <typename T>
  auto getFirstStatementWithType() -> std::optional<const T*> {
    for (const auto& scope : std::views::reverse(mScopes)) {
      for (const auto& statement : std::views::reverse(scope.statements)) {
        if (statement->is<T>()) {
          return &statement->as<T>();
        }
      }
    }
    return std::nullopt;
  }

  static auto typeIsFloat(const LevType&) -> bool;
  static auto typeIsInteger(const LevType&) -> bool;
};

}
