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
