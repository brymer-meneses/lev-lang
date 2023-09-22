#include "scanner.h"
#include "gtest/gtest.h"
#include <ranges>

using namespace lev::scanner;
using namespace lev::token;

TEST(Scanner, SingleCharacterTokens) {
  Scanner scanner("= + - * / > < : !");

  auto tokens = scanner.scan();

  ASSERT_TRUE(tokens.has_value());

  const auto types = {
    TokenType::Equal,
    TokenType::Plus,
    TokenType::Minus,
    TokenType::Star,
    TokenType::Slash,
    TokenType::Greater,
    TokenType::Less,
    TokenType::Colon,
    TokenType::Bang,
  };

  const auto lexemes = {
    "=",
    "+",
    "-",
    "*",
    "/",
    ">",
    "<",
    ":",
    "!",
  };

  for (auto [token, type, lexeme] : std::views::zip(tokens.value(), types, lexemes)) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
  }
}

TEST(Scanner, MultipleCharacterTokens) {
  Scanner scanner("+= -= != /= >= <= *=");

  auto tokens = scanner.scan();

  ASSERT_TRUE(tokens.has_value());

  const auto types = {
    TokenType::PlusEqual,
    TokenType::MinusEqual,
    TokenType::BangEqual,
    TokenType::SlashEqual,
    TokenType::GreaterEqual,
    TokenType::LessEqual,
    TokenType::StarEqual,
  };

  const auto lexemes = {
    "+=",
    "-=",
    "!=",
    "/=",
    ">=",
    "<=",
    "*=",
  };

  for (auto [token, type, lexeme] : std::views::zip(tokens.value(), types, lexemes)) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
  }
}

TEST(Scanner, BuiltInIdentifiers) {
  Scanner scanner("fn pub while for return if else");

  auto tokens = scanner.scan();

  ASSERT_TRUE(tokens.has_value());

  const auto types = {
    TokenType::Function,
    TokenType::Public,
    TokenType::While,
    TokenType::For,
    TokenType::Return,
    TokenType::If,
    TokenType::Else
  };

  const auto lexemes = {
    "fn",
    "pub",
    "while",
    "for",
    "return",
    "if",
    "else",
  };

  for (auto [token, type, lexeme] : std::views::zip(tokens.value(), types, lexemes)) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
  }
}

TEST(Scanner, Number) {
  Scanner scanner("12345 2.71828 3.14159");

  auto tokens = scanner.scan();

  if (not tokens.has_value()) {
    Scanner::printError(tokens.error());
  }

  ASSERT_TRUE(tokens.has_value());

  const auto lexemes = {
    "12345",
    "2.71828",
    "3.14159"
  };

  const auto types = {
    TokenType::Integer,
    TokenType::Float,
    TokenType::Float,
  };

  for (auto [token, type, lexeme] : std::views::zip(tokens.value(), types, lexemes)) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
  }
}

TEST(Scanner, String) {
  Scanner scanner("\" The quick brown fox jumped over the lazy cat! \" \" Hello there! \" ");

  auto tokens = scanner.scan();

  if (not tokens) {
    Scanner::printError(tokens.error());
  }

  ASSERT_TRUE(tokens.has_value());

  const auto lexemes = {
    "\" The quick brown fox jumped over the lazy cat! \"",
    "\" Hello there! \"",
  };

  const auto types = {
    TokenType::String,
    TokenType::String,
  };

  for (auto [token, type, lexeme] : std::views::zip(tokens.value(), types, lexemes)) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
  }
}

TEST(Scanner, Identifier) {
  Scanner scanner("_this_is_a_variable variable");

  auto tokens = scanner.scan();

  if (not tokens) {
    Scanner::printError(tokens.error());
  }

  ASSERT_TRUE(tokens.has_value());

  const auto lexemes = {
    "_this_is_a_variable",
    "variable",
  };

  const auto types = {
    TokenType::Identifier,
    TokenType::Identifier,
  };

  for (auto [token, type, lexeme] : std::views::zip(tokens.value(), types, lexemes)) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
  }
}

TEST(Scanner, Indentation) {
  Scanner scanner(
R"(
fn main(
  a: i32,
  b: i32
) -> i32:
  let num: i32 = 5
  if num == 5:
    if num == 10:
      return 10
    else:
      return 15
    return 5
  else if num == 10:
    return 10
  else:
    return 15
  return 5
)");

  auto tokens = scanner.scan();

  if (not tokens) {
    Scanner::printError(tokens.error());
  }

  ASSERT_TRUE(tokens.has_value());

  const auto types = {
    // fn main(
    TokenType::Newline,
    TokenType::Function,
    TokenType::Identifier,
    TokenType::LeftParen,

    // a: i32,
    TokenType::Identifier,
    TokenType::Colon,
    TokenType::Identifier,
    TokenType::Comma,

    // b: i32
    TokenType::Identifier,
    TokenType::Colon,
    TokenType::Identifier,

    // ) -> i32: \n\t
    TokenType::RightParen,
    TokenType::RightArrow,
    TokenType::Identifier,
    TokenType::Colon,
    TokenType::Indent,
    
    // let num: i32 = 5 \n
    TokenType::Let,
    TokenType::Identifier,
    TokenType::Colon,
    TokenType::Identifier,
    TokenType::Equal,
    TokenType::Integer,
    TokenType::Newline,

    // if num == 5: \n\t
    TokenType::If,
    TokenType::Identifier,
    TokenType::EqualEqual,
    TokenType::Integer,
    TokenType::Colon,
    TokenType::Indent,

    // if num == 10: \n\t
    TokenType::If,
    TokenType::Identifier,
    TokenType::EqualEqual,
    TokenType::Integer,
    TokenType::Colon,
    TokenType::Indent,

    // return 10 \n << \t
    TokenType::Return,
    TokenType::Integer,
    TokenType::Dedent,

    // else: \n\t
    TokenType::Else,
    TokenType::Colon,
    TokenType::Indent,

    // return 15 \n << \t
    TokenType::Return,
    TokenType::Integer,
    TokenType::Dedent,

    // return 5 \n << \t
    TokenType::Return,
    TokenType::Integer,
    TokenType::Dedent,

    // else if num == 10: \n\t
    TokenType::Else,
    TokenType::If,
    TokenType::Identifier,
    TokenType::EqualEqual,
    TokenType::Integer,
    TokenType::Colon,
    TokenType::Indent,
    
    //   return 10 \n << \t
    TokenType::Return,
    TokenType::Integer,
    TokenType::Dedent,

    // else: \n\t
    TokenType::Else,
    TokenType::Colon,
    TokenType::Indent,

    //   return 15 \n << \t
    TokenType::Return,
    TokenType::Integer,
    TokenType::Dedent,
  
    // return 5 \n << \t
    TokenType::Return,
    TokenType::Integer,
    TokenType::Dedent,
  };

  for (auto [token, type] : std::views::zip(tokens.value(), types)) {
    EXPECT_EQ(token.type, type);
  }
}
