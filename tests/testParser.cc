#include <gtest/gtest.h>
#include <lev/parsing/ast.h>
#include <lev/parsing/lexer.h>
#include <lev/parsing/parser.h>

#include "testHelpers.h"

using namespace lev;

TEST(Parser, VariableDeclaration) {
  verifyStatement(
      "let variable: i32 = 5",
      Stmt::VariableDeclaration(
          Token(TokenType::Identifier, "variable", TEST_LOCATION),
          LevType::Builtin::i32(),
          Expr::Literal(Token(TokenType::Integer, "5", TEST_LOCATION))));

  verifyStatement(
      "let variable = 5",
      Stmt::VariableDeclaration(
          Token(TokenType::Identifier, "variable", TEST_LOCATION),
          LevType::Inferred(),
          Expr::Literal(Token(TokenType::Integer, "5", TEST_LOCATION))));
}

TEST(Parser, FunctionDeclaration) {

  auto block = Stmt::Block({});

  block.statements.push_back(
    Stmt::VariableDeclaration(
      Token(TokenType::Identifier, "num", TEST_LOCATION), 
      LevType::Builtin::i32(),
      Expr::Literal(Token(TokenType::Integer, "5", TEST_LOCATION))
    )
  );
  block.statements.push_back(
    Stmt::Return(Expr::Literal(Token(TokenType::Integer, "0", TEST_LOCATION)))
  );

  verifyStatement(
R"(
fn main(a: i32, b: i32) -> i32:
    let num: i32 = 5
    return 0
)",
  Stmt::FunctionDeclaration(
      Token(TokenType::Identifier, "main", TEST_LOCATION),
      { 
        FunctionArgument(Token(TokenType::Identifier, "a", TEST_LOCATION), LevType::Builtin::i32()), 
        FunctionArgument(Token(TokenType::Identifier, "b", TEST_LOCATION), LevType::Builtin::i32()), 
      },
      LevType::Builtin::i32(),
      std::move(block)
    )
  );
}
