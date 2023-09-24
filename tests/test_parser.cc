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

  auto statement = std::move(stmts.value())[0].get();
  auto result = statement->as<VariableDeclaration*>();

  auto expected = VariableDeclaration(
    Token(TokenType::Identifier, "num"),
    false,
    Type::i32,
    std::make_unique<LiteralExpr>(Token(TokenType::Integer, "5"))
  );

  EXPECT_EQ(*result, expected);
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

  auto statement = std::move(stmts.value())[0].get();
  auto result = statement->as<FunctionDeclaration*>();

  auto variableDecl = std::make_unique<VariableDeclaration>(
      Token(TokenType::Identifier, "num"),
      false,
      Type::i32,
      std::make_unique<LiteralExpr>(Token(TokenType::Integer, "5"))
  );

  auto block = std::make_unique<BlockStmt>();
  block->addStmt(std::move(variableDecl));
  
  auto expected = FunctionDeclaration(
    "main",
    {},
    Type::i32,
    std::move(block)
  );

  EXPECT_EQ(*result, expected);
}

TEST(Parser, BinaryExpression) {
  Parser parser("let num: i32 = 1 + 3 + 2 * 2");
  auto stmts = parser.parse();
  if (not stmts) {
    Parser::printError(stmts.error());
  }

  ASSERT_TRUE(stmts.has_value());
  EXPECT_EQ(stmts->size(), 1);

  auto statement = std::move(stmts.value())[0].get();
  auto result = statement->as<VariableDeclaration*>();
  ASSERT_TRUE(result != nullptr);

  auto lhs = std::make_unique<BinaryExpr>(
      std::make_unique<LiteralExpr>(Token(TokenType::Integer, "1")),
      Token(TokenType::Plus, "+"),
      std::make_unique<LiteralExpr>(Token(TokenType::Integer, "3"))
  );
  auto rhs = std::make_unique<BinaryExpr>(
      std::make_unique<LiteralExpr>(Token(TokenType::Integer, "2")),
      Token(TokenType::Star, "*"),
      std::make_unique<LiteralExpr>(Token(TokenType::Integer, "2"))
  );

  auto expected = VariableDeclaration(
    Token(TokenType::Identifier, "num"),
    false,
    Type::i32,
    std::make_unique<BinaryExpr>(std::move(lhs), Token(TokenType::Plus, "+"), std::move(rhs))
  );

  EXPECT_EQ(*result, expected);
}
