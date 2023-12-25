#pragma once

#include "parsing/ast.h"
#include "llvm/IR/IRBuilder.h"

#include <map>
#include <optional>
#include <vector>

namespace lev {

struct Scope {

  std::map<std::string, llvm::AllocaInst*> variables;
  std::vector<const Stmt*> statements;

  auto defineVariable(std::string_view, llvm::AllocaInst*) -> void;
  auto readVariable(std::string_view) -> std::optional<llvm::AllocaInst*>;

  auto addContext(const Stmt*) -> void;

};

}
