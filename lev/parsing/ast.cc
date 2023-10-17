#include <lev/parsing/ast.h>

using namespace lev;

BinaryExpr::BinaryExpr(Expr left, Expr right, Token op) 
  : left(std::make_unique<Expr>(std::move(left)))
  , right(std::make_unique<Expr>(std::move(right)))
  , op(op) {}

UnaryExpr::UnaryExpr(Expr right, Token op)
  : right(std::make_unique<Expr>(std::move(right)))
  , op(op) {}

LiteralExpr::LiteralExpr(Token value) 
  : value(value) {}

VariableDeclaration::VariableDeclaration(Token identifier, LevType type, Expr value) 
  : identifier(identifier)
  , type(type)
  , value(std::move(value)) {}

FunctionArgument::FunctionArgument(Token identifier, LevType type) 
  : identifier(identifier)
  , type(type) {}

FunctionDeclaration::FunctionDeclaration(Token identifier, std::vector<FunctionArgument> arguments, LevType type)
  : identifier(identifier)
  , arguments(std::move(arguments))
  , returnType(type) {}
