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
  std::shared_ptr<llvm::IRBuilder<>> builder;

  auto declareVariable(std::string_view, llvm::Type*) -> void;
  auto assignVariable(std::string_view, llvm::Type*, llvm::Value*) -> void;

  auto readVariable(std::string_view) const -> std::optional<llvm::AllocaInst*>;
  auto addContext(const Stmt*) -> void;

  auto isVariableDefined(std::string_view) const -> bool;

  auto getVariableDeclaration(std::string_view) const -> std::optional<const Stmt::VariableDeclaration*>;

  Scope(std::shared_ptr<llvm::IRBuilder<>> builder) : builder(builder) {};

};

}
