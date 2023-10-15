#include <gtest/gtest.h>
#include <parsing/lexer.h>
#include <print>
#include <ranges>

auto verifyTokens(std::string_view source,
                  const std::initializer_list<const char*> lexemes,
                  const std::initializer_list<TokenType> types) {

  ASSERT_EQ(lexemes.size(), types.size()) << "expected lexemes and types must have the same size";

  Lexer lexer;
  lexer.setSource(source);

  auto tokens = lexer.lex();
  if (not tokens) {
    FAIL() << tokens.error().message();
  }

  ASSERT_EQ(tokens->size(), lexemes.size());

  for (const auto& [token, lexeme, type] : std::views::zip(*tokens, lexemes, types)) {
    EXPECT_EQ(token.lexeme, lexeme) << std::format("Got mismatched lexeme expected: `{}` got: `{}`", lexeme, token.lexeme);
    EXPECT_EQ(token.type, type);
  }
}

TEST(Lexer, SingleCharacterTokens) {

  auto types = {
    TokenType::Equal,
    TokenType::Plus,
    TokenType::Minus,
    TokenType::Bang,
    TokenType::Greater,
    TokenType::Less,
    TokenType::Star,
  };

  auto lexemes = {
    "=",
    "+",
    "-",
    "!",
    ">",
    "<",
    "*",
  };

  verifyTokens("= + - ! > < *", lexemes, types);
}

TEST(Lexer, MultipleCharacterTokens) {

  auto types = {
    TokenType::PlusEqual,
    TokenType::MinusEqual,
    TokenType::GreaterEqual,
    TokenType::LessEqual,
    TokenType::BangEqual,
    TokenType::StarEqual,
    TokenType::SlashEqual,
    TokenType::RightArrow,
    TokenType::EqualEqual,
  };

  auto lexemes = {
    "+=",
    "-=",
    ">=",
    "<=",
    "!=",
    "*=",
    "/=",
    "->",
    "==",
  };

  verifyTokens("+= -= >= <= != *= /= -> ==", lexemes, types);
}

TEST(Lexer, Number) {
  auto types = {
    TokenType::Number,
    TokenType::Number,
    TokenType::Number,
  };

  auto lexemes = {
    "3.141519265",
    "2.71828182",
    "12345",
  };

  verifyTokens("3.141519265 2.71828182 12345", lexemes, types);
}

TEST(Lexer, Identifier) {
  auto types = {
    TokenType::Identifier,
    TokenType::Identifier,
    TokenType::Identifier,
    TokenType::Identifier,
  };

  auto lexemes = {
    "_variable",
    "_variable_with_a_number_12345",
    "ordinaryVariable",
    "variable",
  };

  verifyTokens("_variable _variable_with_a_number_12345 ordinaryVariable variable", lexemes, types);
}

TEST(Lexer, ReservedKeywords) {
  auto types = {
    TokenType::Function,
    TokenType::Mut,
    TokenType::Break,
    TokenType::Class,
    TokenType::For,
    TokenType::If,
    TokenType::Else,
    TokenType::While,
    TokenType::Not,
    TokenType::And,
    TokenType::Or,
    TokenType::Impl,
  };

  auto lexemes = {
    "fn",
    "mut",
    "break",
    "class",
    "for",
    "if",
    "else",
    "while",
    "not",
    "and",
    "or",
    "impl"
  };

  verifyTokens("fn mut break class for if else while not and or impl", lexemes, types);
}

TEST(Lexer, String) {
  auto types = {
    TokenType::String,
    TokenType::String,
  };

  auto lexemes = {
    "\"the quick brown fox jumped over the lazy cat!\"",
    "\"Hello there, someone reading the code!\"",
  };

  verifyTokens("\"the quick brown fox jumped over the lazy cat!\" \"Hello there, someone reading the code!\"", lexemes, types);
}
