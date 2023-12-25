#include "scope.h"
#include "llvm/IR/IRBuilder.h"

using namespace lev;

auto Scope::defineVariable(std::string_view name, llvm::AllocaInst* allocation) -> void {
  variables[std::string(name)] = allocation;
}

auto Scope::readVariable(std::string_view name) -> std::optional<llvm::AllocaInst*> {
  auto name_ = std::string(name);
  if (not variables.contains(name_)) {
    return std::nullopt;
  }
  return variables[std::string(name)];
}

auto Scope::addContext(const Stmt* statement) -> void {
  statements.push_back(statement);
}
