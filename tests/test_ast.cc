#include "ast.h"
#include <gtest/gtest.h>

using namespace lev::ast;
using namespace lev::token;

TEST(AST, Equality) {

  auto l1 = Expr::Literal(Token(TokenType::Integer, "1"));
  auto l2 = Expr::Literal(Token(TokenType::Integer, "1"));

  EXPECT_EQ(l1, l2);
}

