#include "ast.h"

using namespace lev;
using namespace lev::ast;

BlockStmt::BlockStmt(std::vector<Stmt> statements) 
  : statements(std::move(statements)) {}

BlockStmt::BlockStmt(Stmt statement) {
  statements.emplace_back(std::move(statement));
}

AssignStmt::AssignStmt(Token identifier, Expr value) 
  : identifier(identifier),
    value(std::make_unique<Expr>(std::move(value))) {}

ExprStmt::ExprStmt(Expr expr)
    : expr(std::make_unique<Expr>(std::move(expr))) {}

using Branch = IfStmt::Branch;
Branch::Branch(Expr condition, Stmt then)
    : condition(std::move(condition)), 
      body(std::make_unique<Stmt>(std::move(then))) {}

IfStmt::IfStmt(Branch ifBranch, 
               std::vector<Branch> elseIfBranches,
               std::optional<Stmt> elseBody)
    : ifBranch(std::move(ifBranch)), 
      elseIfBranches(std::move(elseIfBranches)) {
  if (elseBody.has_value()) {
    this->elseBody = std::make_unique<Stmt>(std::move(elseBody.value()));
  } else {
    this->elseBody = std::nullopt;
  }
}

FunctionDeclaration::FunctionDeclaration(std::string_view functionName,
                                              std::vector<FunctionArg> args,
                                              Type returnType, Stmt body)
    : functionName(functionName), 
      args(args), 
      returnType(returnType), 
      body(std::make_unique<Stmt>(std::move(body))) {}

VariableDeclaration::VariableDeclaration(Token identifier, bool isMutable, Expr value, Type type)
  : identifier(std::move(identifier)), 
    isMutable(isMutable),
    value(std::make_unique<Expr>(std::move(value))),
    type(type) {}

BinaryExpr::BinaryExpr(Token op, Expr left, Expr right)
  : left(std::make_unique<Expr>(std::move(left))), 
    right(std::make_unique<Expr>(std::move(right))), 
    op(std::move(op)) {}

UnaryExpr::UnaryExpr(Token op, Expr right)
  : right(std::make_unique<Expr>(std::move(right))), 
    op(std::move(op)) {}

LiteralExpr::LiteralExpr(Token value)
  : value(std::move(value)) {}

VariableExpr::VariableExpr(Token identifier)
  : identifier(std::move(identifier)) {}

CallExpr::CallExpr(Token identifier, std::vector<Arg> args)
  : identifier(std::move(identifier)),
    args(std::move(args)) {}

auto ast::operator==(const IfStmt& e1, const IfStmt& e2) -> bool {
  if (e1.elseBody and not e2.elseBody) return false;
  if (not e1.elseBody and e2.elseBody) return false;

  if (e1.elseBody and e2.elseBody) {
    if (*e1.elseBody.value() != *e2.elseBody.value()) return false;
  }

  return e1.ifBranch == e2.ifBranch and e1.elseIfBranches == e2.elseIfBranches;
}

auto ast::operator==(const Branch& e1, const Branch& e2) -> bool {
  return e1.condition == e2.condition and *e1.body == *e2.body;
}

auto ast::operator==(const BlockStmt& e1, const BlockStmt& e2) -> bool {
  if (e2.statements.size() != e1.statements.size()) {
    return false;
  }
  for (auto i=0; i < e1.statements.size(); i++) {
    if (e1.statements[i] != e2.statements[i]) {
      return false;
    };
  }
  return true;
}

auto ast::operator==(const AssignStmt& e1, const AssignStmt& e2) -> bool {
  return e1.identifier == e2.identifier and *e1.value == *e2.value;
}

auto ast::operator==(const ExprStmt& e1, const ExprStmt& e2) -> bool {
  return *e1.expr == *e2.expr;
}

auto ast::operator==(const FunctionDeclaration& e1, const FunctionDeclaration& e2) -> bool {
  return e1.functionName == e2.functionName and
         e1.args == e2.args and
         e1.returnType == e2.returnType and
         *e1.body == *e2.body;
}

auto ast::operator==(const VariableDeclaration& e1, const VariableDeclaration& e2) -> bool {
  return e1.identifier == e2.identifier and 
         e1.isMutable == e2.isMutable and
         *e1.value == *e2.value and
         e1.type == e2.type;
}

auto ast::operator==(const Stmt& e1, const Stmt& e2) -> bool {
  return e1.mData == e2.mData;
}

auto ast::operator==(const BinaryExpr& e1, const BinaryExpr& e2) -> bool {
  return *e1.left == *e2.left and *e1.right == *e2.right and e1.op == e2.op;
}

auto ast::operator==(const UnaryExpr& e1, const UnaryExpr& e2) -> bool {
  return *e1.right == *e2.right and e1.op == e2.op;
}

auto ast::operator==(const VariableExpr& e1, const VariableExpr& e2) -> bool {
  return e1.identifier == e2.identifier;
}

auto ast::operator==(const LiteralExpr& e1, const LiteralExpr& e2) -> bool {
  return e1.value == e2.value;
}

auto ast::operator==(const CallExpr& e1, const CallExpr& e2) -> bool {
  return e2.identifier == e1.identifier and e2.args == e2.args;
}

auto ast::operator==(const Expr& e1, const Expr& e2) -> bool {
  return e1.mData == e2.mData;
}
