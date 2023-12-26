#include <lev/parsing/ast.h>

using namespace lev;

Expr::Binary::Binary(Expr left, Expr right, Token op) 
  : left(std::make_unique<Expr>(std::move(left)))
  , right(std::make_unique<Expr>(std::move(right)))
  , op(op) {}

Expr::Unary::Unary(Expr right, Token op)
  : right(std::make_unique<Expr>(std::move(right)))
  , op(op) {}

Expr::Literal::Literal(Token value) 
  : value(value) {}

Stmt::VariableDeclaration::VariableDeclaration(Token identifier, LevType type, Expr value, bool isMutable) 
  : identifier(identifier)
  , type(type)
  , value(std::move(value))
  , isMutable(isMutable) {}

Stmt::FunctionDeclaration::FunctionDeclaration(
    Token identifier, 
    std::vector<FunctionArgument> arguments, 
    LevType type,
    Stmt body)
  : identifier(identifier)
  , arguments(std::move(arguments))
  , returnType(type) 
  , body(std::make_unique<Stmt>(std::move(body))) {}

Stmt::Block::Block(std::vector<Stmt> statements) 
  : statements(std::move(statements)) {} 

Stmt::Return::Return(Expr expr) 
  : expr(std::move(expr)) {} 

Stmt::Control::Control(Branch ifBranch, 
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

Stmt::Assignment::Assignment(Token identifier, Expr value) 
  : identifier(identifier),
    value(std::move(value)) {}


FunctionArgument::FunctionArgument(Token identifier, LevType type) 
  : identifier(identifier)
  , type(type) {}

Branch::Branch(Expr condition, Stmt then)
    : condition(std::move(condition)), 
      body(std::make_unique<Stmt>(std::move(then))) {}
