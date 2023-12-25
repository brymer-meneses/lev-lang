#include "scope.h"
#include "llvm/IR/IRBuilder.h"

using namespace lev;

auto Scope::declareVariable(std::string_view name, llvm::Type* type) -> void {
  variables[std::string(name)] = builder->CreateAlloca(type, nullptr, name);
}

auto Scope::assignVariable(std::string_view name, llvm::Type* type, llvm::Value* value) -> void {
  auto name_ = std::string(name);
  if (not variables.contains(name_)) {
    declareVariable(name_, type);
  }
  builder->CreateStore(value, variables.at(name_));
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
