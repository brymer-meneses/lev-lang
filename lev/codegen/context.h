#pragma once

#include "codegen/scope.h"
#include <stack>
namespace lev {

class Context {
private:
  
  std::stack<Scope> mScopes;

public:
  auto createScope() -> Scope&;
  auto getCurrentScope() -> Scope&;
  auto popCurrentScope() -> void;

  auto getCurrentStatement() -> const Stmt*;

  auto getAppropriateExprType() -> std::optional<LevType>;
};

}
