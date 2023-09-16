#include "scanner.h"
#include "gtest/gtest.h"
#include <ranges>

using namespace lev::scanner;

TEST(Scanner, SingleCharacterTokens) {
  using T = lev::TokenType;
  Scanner scanner("= + - * / > < : !");

  auto tokens = scanner.scan();

  ASSERT_TRUE(tokens.has_value());

  const auto types = {
    T::Equal,
    T::Plus,
    T::Minus,
    T::Star,
    T::Slash,
    T::Greater,
    T::Less,
    T::Colon,
    T::Bang,
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
  using T = lev::TokenType;
  Scanner scanner("+= -= != /= >= <= *=");

  auto tokens = scanner.scan();

  ASSERT_TRUE(tokens.has_value());

  const auto types = {
    T::PlusEqual,
    T::MinusEqual,
    T::BangEqual,
    T::SlashEqual,
    T::GreaterEqual,
    T::LessEqual,
    T::StarEqual,
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
  using T = lev::TokenType;
  Scanner scanner("fn pub while for");

  auto tokens = scanner.scan();

  ASSERT_TRUE(tokens.has_value());

  const auto types = {
    T::Function,
    T::Public,
    T::While,
    T::For
  };

  const auto lexemes = {
    "fn",
    "pub",
    "while",
    "for"
  };

  for (auto [token, type, lexeme] : std::views::zip(tokens.value(), types, lexemes)) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
  }
}

TEST(Scanner, Number) {
  using T = lev::TokenType;
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
    T::Integer,
    T::Float,
    T::Float,
  };

  for (auto [token, type, lexeme] : std::views::zip(tokens.value(), types, lexemes)) {
    EXPECT_EQ(token.type, type);
    EXPECT_EQ(token.lexeme, lexeme);
  }
}
