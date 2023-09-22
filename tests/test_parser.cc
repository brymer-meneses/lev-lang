#include <gtest/gtest.h>

#include "parser.h"
#include <memory>

using namespace lev::parser;

TEST(Parser, VariableDeclaration) {
  Parser parser("let num: i32 = 5");

  auto stmts = parser.parse();

  ASSERT_TRUE(stmts.has_value());

}
