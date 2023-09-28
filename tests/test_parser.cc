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

  auto expected = VariableDeclaration(
    Token(TokenType::Identifier, "num"),
    false,
    LiteralExpr(Token(TokenType::Integer, "5")),
    Type::i32
  );

  EXPECT_EQ(statement, Stmt(std::move(expected)));
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

  auto expected = FunctionDeclaration(
    "main", 
    {}, 
    Type::i32,
    BlockStmt(
      VariableDeclaration(
      Token(TokenType::Identifier, "num"), 
      false,
      LiteralExpr(Token(TokenType::Integer, "5")), 
      Type::i32))
  );

  EXPECT_EQ(statement, Stmt(std::move(expected)));
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

  auto lhs = BinaryExpr(
      Token(TokenType::Plus, "+"),
      LiteralExpr(Token(TokenType::Integer, "1")),
      LiteralExpr(Token(TokenType::Integer, "3"))
  );
  auto rhs = BinaryExpr(
      Token(TokenType::Star, "*"),
      LiteralExpr(Token(TokenType::Integer, "2")),
      LiteralExpr(Token(TokenType::Integer, "2"))
  );

  auto expected = VariableDeclaration(
    Token(TokenType::Identifier, "num"),
    false,
    BinaryExpr(Token(TokenType::Plus, "+"), std::move(lhs), std::move(rhs)),
    Type::i32
  );

  EXPECT_EQ(statement, Stmt(std::move(expected)));
}

TEST(Parser, AssignmentStmt) {
  Parser parser("variable = 5");
  auto stmts = parser.parse();

  if (not stmts) {
    Parser::printError(stmts.error());
  }

  ASSERT_TRUE(stmts.has_value());
  EXPECT_EQ(stmts->size(), 1);

  auto statement = std::move(stmts.value()[0]);
  auto expected = AssignStmt(
    Token(TokenType::Identifier, "variable"),
    LiteralExpr(Token(TokenType::Integer, "5"))
  );

  EXPECT_EQ(statement, Stmt(std::move(expected)));
}

TEST(Parser, IfStmt) {
  GTEST_SKIP();

  Parser parser(
R"(
fn main() -> i32:
  if true:
    let variable: i32 = 1
  else if true:
    let variable: i32 = 2
  else if true:
    let variable: i32 = 3
  else:
    let variable: i32 = 4
)");
  auto stmts = parser.parse();
  if (not stmts) {
    Parser::printError(stmts.error());
  }

  ASSERT_TRUE(stmts);
  EXPECT_EQ(stmts->size(), 1);

  auto statement = std::move(stmts.value()[0]);

  std::vector<IfStmt::Branch> elseIfBranches;

  elseIfBranches.push_back(
    IfStmt::Branch(
      LiteralExpr(Token(TokenType::True, "true")),
      BlockStmt(
        VariableDeclaration(
          Token(TokenType::Identifier, "variable"),
          false,
          LiteralExpr(Token(TokenType::Integer, "1")),
          Type::i32
        )
      )
    )
  );

  elseIfBranches.push_back(
    IfStmt::Branch(
      LiteralExpr(Token(TokenType::True, "true")),
      BlockStmt(
        VariableDeclaration(
          Token(TokenType::Identifier, "variable"),
          false,
          LiteralExpr(Token(TokenType::Integer, "2")),
          Type::i32
        )
      )
    )
  );

  auto expected = FunctionDeclaration(
      "main", {},
      Type::i32,
      IfStmt(
        IfStmt::Branch(
          LiteralExpr(Token(TokenType::True, "true")),
          BlockStmt(
            VariableDeclaration(
            Token(TokenType::Identifier, "variable"),
            false,
            LiteralExpr(Token(TokenType::Integer, "3")),
            Type::i32
          ))
      ),
      std::move(elseIfBranches),
      BlockStmt(
        VariableDeclaration(
          Token(TokenType::Identifier, "variable"),
          false,
          LiteralExpr(Token(TokenType::Integer, "4")),
          Type::i32
        )
      )
    )
  );

  EXPECT_EQ(statement, Stmt(std::move(expected)));
}
