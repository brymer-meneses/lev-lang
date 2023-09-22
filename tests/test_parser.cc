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

  std::vector<std::unique_ptr<Stmt>> body;
  body.push_back(std::move(variableDecl));

  std::vector<FunctionArg> args = {};
  auto expected = FunctionDeclaration{
    "main",
    {},
    std::move(body),
    Type::i32,
  };

  EXPECT_EQ(*result, expected);
}