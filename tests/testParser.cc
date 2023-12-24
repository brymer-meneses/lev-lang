#include <gtest/gtest.h>
#include <lev/parsing/ast.h>
#include <lev/parsing/lexer.h>
#include <lev/parsing/parser.h>

#include "testHelpers.h"

using namespace lev;

TEST(Parser, BinaryExpression) {
  auto source = "let num: i32 = 1 + 3 + 2 * 2";

  auto lhs = Expr::Binary(
      Expr::Literal(Token(TokenType::Integer, "1", TEST_LOCATION)),
      Expr::Literal(Token(TokenType::Integer, "3", TEST_LOCATION)),
      Token(TokenType::Plus, "+", TEST_LOCATION)
  );
  auto rhs = Expr::Binary(
      Expr::Literal(Token(TokenType::Integer, "2", TEST_LOCATION)),
      Expr::Literal(Token(TokenType::Integer, "2", TEST_LOCATION)),
      Token(TokenType::Star, "*", TEST_LOCATION)
  );

  auto expected = Stmt::VariableDeclaration(
    Token(TokenType::Identifier, "num", TEST_LOCATION),
    LevType::Builtin::i32(),
    Expr::Binary( std::move(lhs), std::move(rhs), Token(TokenType::Plus, "+", TEST_LOCATION))
  );

  verifyStatement(source, std::move(expected));
}

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

TEST(Parser, ControlStatement) {

  auto source = 
R"(
if num == 1:
  let variable: i32 = 1
else if num == 2:
  let variable: i32 = 2
else if num == 3:
  let variable: i32 = 3
else:
  let variable: i32 = 4
)";

  auto ifBlock = Stmt::Block({});
  ifBlock.statements.push_back(
    Stmt::VariableDeclaration(
      Token(TokenType::Identifier, "variable", TEST_LOCATION), 
      LevType::Builtin::i32(),
      Expr::Literal(Token(TokenType::Integer, "1", TEST_LOCATION))
    )
  );

  auto elseIfBlock1 = Stmt::Block({});
  elseIfBlock1.statements.push_back(
    Stmt::VariableDeclaration(
      Token(TokenType::Identifier, "variable", TEST_LOCATION), 
      LevType::Builtin::i32(),
      Expr::Literal(Token(TokenType::Integer, "2", TEST_LOCATION))
    )
  );

  auto elseIfBlock2 = Stmt::Block({});
  elseIfBlock2.statements.push_back(
    Stmt::VariableDeclaration(
      Token(TokenType::Identifier, "variable", TEST_LOCATION), 
      LevType::Builtin::i32(),
      Expr::Literal(Token(TokenType::Integer, "3", TEST_LOCATION))
    )
  );

  auto elseBlock = Stmt::Block({});
  elseBlock.statements.push_back(
    Stmt::VariableDeclaration(
      Token(TokenType::Identifier, "variable", TEST_LOCATION), 
      LevType::Builtin::i32(),
      Expr::Literal(Token(TokenType::Integer, "4", TEST_LOCATION))
    )
  );

  auto elseIfBranches = std::vector<Branch> {};
  elseIfBranches.push_back(
    Branch(
      Expr::Binary(
        Expr::Literal(Token(TokenType::Identifier, "num", TEST_LOCATION)),
        Expr::Literal(Token(TokenType::Integer, "2", TEST_LOCATION)),
        Token(TokenType::EqualEqual, "==", TEST_LOCATION)
      ),
      std::move(elseIfBlock1)
    )
  );
  elseIfBranches.push_back(
    Branch(
      Expr::Binary(
        Expr::Literal(Token(TokenType::Identifier, "num", TEST_LOCATION)),
        Expr::Literal(Token(TokenType::Integer, "3", TEST_LOCATION)),
        Token(TokenType::EqualEqual, "==", TEST_LOCATION)
      ),
      std::move(elseIfBlock2)
    )
  );

  auto statement = Stmt::Control(
    Branch(
      Expr::Binary(
        Expr::Literal(Token(TokenType::Identifier, "num", TEST_LOCATION)),
        Expr::Literal(Token(TokenType::Integer, "1", TEST_LOCATION)),
        Token(TokenType::EqualEqual, "==", TEST_LOCATION)),
      std::move(ifBlock)
    ),
    std::move(elseIfBranches),
    std::move(elseBlock)
  );


  verifyStatement(source, std::move(statement));
}
