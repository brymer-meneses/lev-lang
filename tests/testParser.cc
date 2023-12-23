#include <gtest/gtest.h>
#include <lev/parsing/ast.h>
#include <lev/parsing/lexer.h>
#include <lev/parsing/parser.h>

#include "equalityHelper.h"
#include "testHelpers.h"

using namespace lev;

TEST(Parser, VariableDeclaration) {
  verifyStatement(
      "let variable: i32 = 5",
      VariableDeclarationStmt(
          Token(TokenType::Identifier, "variable", TEST_LOCATION),
          LevType::Builtin::i32(),
          LiteralExpr(Token(TokenType::Integer, "5", TEST_LOCATION))));

  verifyStatement(
      "let variable = 5",
      VariableDeclarationStmt(
          Token(TokenType::Identifier, "variable", TEST_LOCATION),
          LevType::Inferred(),
          LiteralExpr(Token(TokenType::Integer, "5", TEST_LOCATION))));
}
