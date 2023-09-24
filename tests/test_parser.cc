#include <gtest/gtest.h>

#include "parser.h"
#include <memory>

using namespace lev::ast;
using namespace lev::token;
using namespace lev::parser;

TEST(Parser, VariableDeclaration) {
  Parser parser("let num: i32 = 5");

  auto stmts = parser.parse();

  if (not stmts) {
    Parser::printError(stmts.error());
  }

  ASSERT_TRUE(stmts.has_value());
  EXPECT_EQ(stmts->size(), 1);

  auto statement = std::move(stmts.value()[0]);

  auto expected = Stmt::VariableDeclaration(
    Token(TokenType::Identifier, "num"),
    false,
    Expr::Literal(Token(TokenType::Integer, "5")),
    Type::i32
  );

  EXPECT_EQ(statement, expected);
}

TEST(Parser, FunctionDeclaration) {
  Parser parse(
R"(
fn main() -> i32:
    let num: i32 = 5
)"
  );

  auto stmts = parse.parse();

  if (not stmts) {
    Parser::printError(stmts.error());
  }

  ASSERT_TRUE(stmts.has_value());
  EXPECT_EQ(stmts->size(), 1);

  auto statement = std::move((*stmts)[0]);
  auto variableDeclaration = Stmt::VariableDeclaration(
      Token(TokenType::Identifier, "num"), 
      false,
      Expr::Literal(Token(TokenType::Integer, "5")), 
      Type::i32
  );

  std::vector<Stmt> body;
  body.emplace_back(std::move(variableDeclaration));

  auto expected = Stmt::FunctionDeclaration(
      "main", 
      {}, 
      Type::i32,
      Stmt::Block(std::move(body)));

  EXPECT_EQ(statement, expected);
}

TEST(Parser, BinaryExpression) {
  Parser parser("let num: i32 = 1 + 3 + 2 * 2");
  auto stmts = parser.parse();
  if (not stmts) {
    Parser::printError(stmts.error());
  }

  ASSERT_TRUE(stmts.has_value());
  EXPECT_EQ(stmts->size(), 1);

  auto statement = std::move(stmts.value()[0]);

  auto lhs = Expr::Binary(
      Token(TokenType::Plus, "+"),
      Expr::Literal(Token(TokenType::Integer, "1")),
      Expr::Literal(Token(TokenType::Integer, "3"))
  );
  auto rhs = Expr::Binary(
      Token(TokenType::Star, "*"),
      Expr::Literal(Token(TokenType::Integer, "2")),
      Expr::Literal(Token(TokenType::Integer, "2"))
  );

  auto expected = Stmt::VariableDeclaration(
    Token(TokenType::Identifier, "num"),
    false,
    Expr::Binary(Token(TokenType::Plus, "+"), std::move(lhs), std::move(rhs)),
    Type::i32
  );

  EXPECT_EQ(statement, expected);
}
