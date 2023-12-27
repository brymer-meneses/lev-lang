#include <gtest/gtest.h>
#include <lev/parsing/ast.h>
#include <lev/parsing/lexer.h>
#include <lev/parsing/parser.h>

#include "testHelpers.h"

using namespace lev;

#define NODE(type, ...) \
  std::make_unique<type>(__VA_ARGS__) \

#define TOKEN(type, lexeme) \
  Token(TokenType::type, lexeme, TEST_LOCATION)
  
TEST(Parser, BinaryExpression) {
  auto source = "let num: i32 = 1 + 3 + 2 * 2";

  auto lhs = NODE(BinaryExpr, 
      NODE(LiteralExpr, TOKEN(Integer, "1")),
      NODE(LiteralExpr, TOKEN(Integer, "3")),
      TOKEN(Plus, "+"));

  auto rhs = NODE(BinaryExpr,
      NODE(LiteralExpr, TOKEN(Integer, "2")),
      NODE(LiteralExpr, TOKEN(Integer, "2")),
      TOKEN(Star, "*"));

  auto expected = NODE(VariableDeclaration,
    TOKEN(Identifier, "num"),
    LevType::Builtin::i32(),
    NODE(BinaryExpr, std::move(lhs), std::move(rhs), TOKEN(Plus, "+")),
    false
  );

  verifyStatement(source, *expected);
}

TEST(Parser, VariableDeclaration) {
  verifyStatement(
      "let mut variable: i32 = 5",
      *NODE(VariableDeclaration,
          TOKEN(Identifier, "variable"),
          LevType::Builtin::i32(),
          NODE(LiteralExpr, TOKEN(Integer, "5")),
          true
    ));

  verifyStatement(
      "let variable = 5",
      *NODE(VariableDeclaration,
          TOKEN(Identifier, "variable"),
          LevType::Inferred(),
          NODE(LiteralExpr, TOKEN(Integer, "5")),
          false
    ));
}

TEST(Parser, FunctionDeclaration) {

  auto statement = NODE(
      FunctionDeclaration,
      TOKEN(Identifier, "main"),
      std::vector<FunctionArgument> {
          FunctionArgument(TOKEN(Identifier, "a"), LevType::Builtin::i32()),
          FunctionArgument(TOKEN(Identifier, "b"), LevType::Builtin::i32()),
      },
      LevType::Builtin::i32(), 
      NODE(BlockStmt, 
            NODE(VariableDeclaration,
              TOKEN(Identifier, "num"),
              LevType::Inferred(),
              NODE(LiteralExpr, TOKEN(Integer, "5")),
              false
            ),
            NODE(ReturnStmt, NODE(IdentifierExpr, TOKEN(Identifier, "num")))
    ));

  verifyStatement(
R"(
  fn main(a: i32, b: i32) -> i32:
      let num: i32 = 5
      return num
  )", *statement);
}

TEST(Parser, Assignment) {
  auto source = "variable = 5";

  auto expected = NODE(AssignmentStmt,
    TOKEN(Identifier, "variable"),
    NODE(LiteralExpr, TOKEN(Integer, "5")));

  verifyStatement(source, *expected);
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

  auto statement = 
    NODE(ControlStmt,
      Branch(
        NODE(BinaryExpr, 
           NODE(IdentifierExpr, TOKEN(Identifier, "num")),
           NODE(LiteralExpr, TOKEN(Integer, "1")),
           TOKEN(EqualEqual, "==")
        ),
        NODE(BlockStmt, 
             NODE(VariableDeclaration,
                TOKEN(Identifier, "variable"),
                LevType::Builtin::i32(),
                NODE(LiteralExpr, TOKEN(Integer, "1")),
                false
              )
        )
      ),
      NODE(BlockStmt, 
         NODE(VariableDeclaration,
            TOKEN(Identifier, "variable"),
            LevType::Builtin::i32(),
            NODE(LiteralExpr, TOKEN(Integer, "4")),
            false
          )
      ),
     Branch(
        NODE(BinaryExpr, 
             NODE(IdentifierExpr, TOKEN(Identifier, "num")),
             NODE(LiteralExpr, TOKEN(Integer, "2")),
             TOKEN(EqualEqual, "==")),
        NODE(BlockStmt, 
             NODE(VariableDeclaration,
                TOKEN(Identifier, "variable"),
                LevType::Builtin::i32(),
                NODE(LiteralExpr, TOKEN(Integer, "2")),
                false
              )
        )
     ),
     Branch(
        NODE(BinaryExpr, 
             NODE(IdentifierExpr, TOKEN(Identifier, "num")),
             NODE(LiteralExpr, TOKEN(Integer, "3")),
             TOKEN(EqualEqual, "==")),
        NODE(BlockStmt, 
             NODE(VariableDeclaration,
                TOKEN(Identifier, "variable"),
                LevType::Builtin::i32(),
                NODE(LiteralExpr, TOKEN(Integer, "3")),
                false
              )
        )
     )
  );

  verifyStatement(source, *statement);
}
