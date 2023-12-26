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

auto Scope::readVariable(std::string_view name) const -> std::optional<llvm::AllocaInst*> {
  auto name_ = std::string(name);
  if (not variables.contains(name_)) {
    return std::nullopt;
  }
  return variables.at(name_);
}

auto Scope::addContext(const Stmt* statement) -> void {
  statements.push_back(statement);
}

auto Scope::isVariableDefined(std::string_view name) const -> bool {
  return variables.contains(std::string(name));
}

auto Scope::getVariableDeclaration(std::string_view name) const -> std::optional<const Stmt::VariableDeclaration*> {
  auto condition = [&name](const Stmt* s) {
    if (s->is<Stmt::VariableDeclaration>()) {
      return s->as<Stmt::VariableDeclaration>().identifier.lexeme == name;
    }
    return false;
  };
  auto it = std::ranges::find_if(statements, condition);
  if (it == statements.end()) {
    return std::nullopt;
  }
  return &(*it)->as<Stmt::VariableDeclaration>();
}

