#include <lev/parsing/ast.h>
#include <ranges>

using namespace lev;

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, Token op) 
  : left(std::move(left))
  , right(std::move(right))
  , op(op) {
  this->left->setParent(this);
  this->right->setParent(this);
}

UnaryExpr::UnaryExpr(std::unique_ptr<Expr> right, Token op)
  : right(std::move(right))
  , op(op) {
  this->right->setParent(this);
}

LiteralExpr::LiteralExpr(Token value) 
  : value(value) {}

IdentifierExpr::IdentifierExpr(Token identifier) 
  : identifier(identifier) {}

VariableDeclaration::VariableDeclaration(Token identifier, LevType type, std::unique_ptr<Expr> value, bool isMutable) 
  : identifier(identifier)
  , type(type)
  , value(std::move(value))
  , isMutable(isMutable) {
}

FunctionDeclaration::FunctionDeclaration(
    Token identifier, 
    std::vector<FunctionArgument> arguments, 
    LevType type,
    std::unique_ptr<Stmt> body)
  : identifier(identifier)
  , arguments(std::move(arguments))
  , returnType(type) 
  , body(std::move(body)) {

  this->body->setParent(this);

  for (auto& arg : this->arguments) {
    arg.setParent(this);
  }
}

BlockStmt::BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) 
  : statements(std::move(statements)) {

  for (auto& statement : this->statements) {
    statement->setParent(this);
  }
} 

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> expr) 
  : expr(std::move(expr)) {

  this->expr->setParent(this);
} 

ControlStmt::ControlStmt(Branch ifBranch, 
              std::unique_ptr<Stmt> elseBody,
               std::vector<Branch> elseIfBranches)
    : ifBranch(std::move(ifBranch))
    , elseIfBranches(std::move(elseIfBranches)) 
    , elseBody(std::move(elseBody)) {

  this->ifBranch.setParent(this);
  this->elseBody->setParent(this);

  for (auto& branch : this->elseIfBranches) {
    branch.setParent(this);
  }

}

AssignmentStmt::AssignmentStmt(Token identifier, std::unique_ptr<Expr> value) 
  : identifier(identifier),
    value(std::move(value)) {
  this->value->setParent(this);
}


FunctionArgument::FunctionArgument(Token identifier, LevType type) 
  : identifier(identifier)
  , type(type) {}

Branch::Branch(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then)
    : condition(std::move(condition)), 
      body(std::move(then)) {

  this->condition->setParent(this);
  this->body->setParent(this);
}

auto Branch::operator==(const Branch& s) const -> bool {
  return *this->body == *s.body and *this->condition == *s.condition;
}

auto FunctionArgument::operator==(const FunctionArgument& s) const -> bool {
  return this->identifier == s.identifier and this->type == s.type;
}

auto BinaryExpr::operator==(const Expr& e) const -> bool {
  if (auto casted = dynamic_cast<const BinaryExpr*>(&e)) {
    return this->op == casted->op and *this->left == *casted->left and *this->right == *casted->right;
  }
  return false;
}

auto UnaryExpr::operator==(const Expr& e) const -> bool {
  if (auto casted = dynamic_cast<const UnaryExpr*>(&e)) {
    return this->op == casted->op and *this->right == *casted->right;
  }
  return false;
}

auto IdentifierExpr::operator==(const Expr& e) const -> bool {
  if (auto casted = dynamic_cast<const IdentifierExpr*>(&e)) {
    return this->identifier == casted->identifier;
  }
  return false;
}

auto LiteralExpr::operator==(const Expr& e) const -> bool {
  if (auto casted = dynamic_cast<const LiteralExpr*>(&e)) {
    return this->value == casted->value;
  }
  return false;
}

auto VariableDeclaration::operator==(const Stmt& s) const -> bool {
  if (auto casted = dynamic_cast<const VariableDeclaration*>(&s)) {
    return this->identifier == casted->identifier and
           *this->value == *casted->value and
           this->type == casted->type and
           this->isMutable == casted->isMutable;
  }
  return false;
}

auto FunctionDeclaration::operator==(const Stmt& e) const -> bool {
  if (auto casted = dynamic_cast<const FunctionDeclaration*>(&e)) {
    return this->identifier == casted->identifier and
           this->arguments == casted->arguments and
           this->returnType == casted->returnType and
           *this->body == *casted->body;
  }
  return false;
}

auto BlockStmt::operator==(const Stmt& s) const -> bool {
  if (auto casted = dynamic_cast<const BlockStmt*>(&s)) {
    if (casted->statements.size() != this->statements.size()) return false;
    for (const auto& [s1, s2] : std::ranges::views::zip(this->statements, casted->statements)) {
      if (*s1 != *s2) {
        return false;
      }
    }
    return true;
  }
  return false;
}

auto ControlStmt::operator==(const Stmt& e) const -> bool {
  if (auto casted = dynamic_cast<const ControlStmt*>(&e)) {
    if (this->elseBody == nullptr and casted->elseBody != nullptr)
      return false;

    if (this->elseBody != nullptr and casted->elseBody == nullptr)
      return false;

    if (this->elseBody) {
      return this->ifBranch == casted->ifBranch and
             this->elseIfBranches == casted->elseIfBranches and
             *this->elseBody == *casted->elseBody;
    } else {
      return this->ifBranch == casted->ifBranch and
             this->elseIfBranches == casted->elseIfBranches;
    }
    
  }
  return false;
}

auto ReturnStmt::operator==(const Stmt& e) const -> bool {
  if (auto casted = dynamic_cast<const ReturnStmt*>(&e)) {
    return *this->expr == *casted->expr;
  }
  return false;
}

auto AssignmentStmt::operator==(const Stmt& e) const -> bool {
  if (auto casted = dynamic_cast<const AssignmentStmt*>(&e)) {
    return *this->value == *casted->value and this->identifier == casted->identifier;
  }
  return false;
}
