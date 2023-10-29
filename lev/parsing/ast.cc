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

Stmt::VariableDeclaration::VariableDeclaration(Token identifier, LevType type, Expr value) 
  : identifier(identifier)
  , type(type)
  , value(std::move(value)) {}

Stmt::FunctionArgument::FunctionArgument(Token identifier, LevType type) 
  : identifier(identifier)
  , type(type) {}

Stmt::FunctionDeclaration::FunctionDeclaration(
    Token identifier, 
    std::vector<FunctionArgument> arguments, 
    LevType type)
  : identifier(identifier)
  , arguments(std::move(arguments))
  , returnType(type) {}
