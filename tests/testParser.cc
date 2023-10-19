#include <gtest/gtest.h>
#include <lev/parsing/ast.h>
#include <lev/parsing/lexer.h>
#include <lev/parsing/parser.h>

#include "equalityHelper.h"

using namespace lev;

auto verifyStatement(std::string_view source, const Stmt& expectedStatement) -> void {
  Lexer lexer;
  lexer.setSource(source);
  auto tokens = lexer.lex();

  ASSERT_TRUE(tokens) << tokens.error().message();

  Parser parser;
  parser.setTokens(std::move(*tokens));
  auto statements = parser.parse();

  ASSERT_TRUE(statements) << statements.error().message();
  ASSERT_EQ(statements->size(), 1);

  EXPECT_EQ(statements->at(0), expectedStatement);
}

static SourceLocation DUMMY_SOURCE_LOCATION("testing.lev", 0, 0, 1);

TEST(Parser, VariableDeclaration) {
  verifyStatement(
      "let variable: i32 = 5",
      VariableDeclaration(
          Token(TokenType::Identifier, "variable", DUMMY_SOURCE_LOCATION),
          LevType::Builtin::i32(),
          LiteralExpr(Token(TokenType::Number, "5", DUMMY_SOURCE_LOCATION))));

  verifyStatement(
      "let variable = 5",
      VariableDeclaration(
          Token(TokenType::Identifier, "variable", DUMMY_SOURCE_LOCATION),
          LevType::Inferred(),
          LiteralExpr(Token(TokenType::Number, "5", DUMMY_SOURCE_LOCATION))));
}

TEST(Parser, FunctionDeclaration) {

}

