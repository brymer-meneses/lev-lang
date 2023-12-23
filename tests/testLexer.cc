#include <gtest/gtest.h>
#include <parsing/lexer.h>
#include <print>
#include <ranges>

using namespace lev;

#include "testHelpers.h"

TEST(Lexer, SingleCharacterTokens) {

  auto types = {
    TokenType::Equal,
    TokenType::Plus,
    TokenType::Minus,
    TokenType::Bang,
    TokenType::Greater,
    TokenType::Less,
    TokenType::Star,
    TokenType::Colon,
    TokenType::Semicolon,
  };

  auto lexemes = {
    "=",
    "+",
    "-",
    "!",
    ">",
    "<",
    "*",
    ":",
    ";"
  };

  verifyTokens("= + - ! > < * : ;", lexemes, types);
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
    TokenType::Float,
    TokenType::Float,
    TokenType::Integer,
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
    TokenType::Mutable,
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
    TokenType::True,
    TokenType::False,
    TokenType::Return,
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
    "impl",
    "true",
    "false",
    "return",
  };

  verifyTokens("fn mut break class for if else while not and or impl true false return", lexemes, types);
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

TEST(Lexer, SkipComments) {
  auto types = {
    TokenType::Function,
    TokenType::If,
  };

  auto lexemes = {
    "fn",
    "if"
  };

  verifyTokens("// this should be ignored\nfn if", lexemes, types);
}

TEST(Lexer, Indentation) {

  const auto types = {
    TokenType::If,
    TokenType::Identifier,
    TokenType::EqualEqual,
    TokenType::Integer,
    TokenType::Colon,

    TokenType::Indent,
    TokenType::If,
    TokenType::Identifier,
    TokenType::EqualEqual,
    TokenType::Integer,
    TokenType::Colon,

    TokenType::Indent,
    TokenType::Return,
    TokenType::Integer,

    TokenType::Dedent,
    TokenType::Return,
    TokenType::Integer,
  };

  // FIXME: we're not parsing statements like this
  // fn add(
  //  a: i32,
  //  b: i32
  // ) -> i32
  // yet

  const std::string_view source = \
R"(
if num == 5:
  if num == 10:
    return 10
return 5
)";


  verifyTokens(source, types);
}
