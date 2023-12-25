#pragma once

#include "codegen/scope.h"
#include <stack>
namespace lev {

class Context {
private:
  
  std::stack<Scope> mScopes;
  std::shared_ptr<llvm::IRBuilder<>> mBuilder;

public:
  Context(std::shared_ptr<llvm::IRBuilder<>> builder) : mBuilder(builder) {}
  Context() = default;

  auto createScope() -> Scope&;
  auto getCurrentScope() -> Scope&;
  auto popCurrentScope() -> void;

  auto getCurrentStatement() -> const Stmt*;

  auto getAppropriateExprType() -> std::optional<LevType>;
};

}
