#pragma once
#include "../sourceLocation.h"

enum class TokenType {
  Number,
  String,
  Identifier,

  While,
  For,
  If,
  Else,
  Break,
  Class,
  Not,
  And,
  Or,

  Function,
  Mut,
  Impl,

  Plus,
  Minus,
  Star,
  Slash,
  Bang,

  RightArrow,

  PlusEqual,
  MinusEqual,
  StarEqual,
  SlashEqual,

  LeftParen,
  RightParen,
  LeftBracket,
  RightBracket,
  LeftBrace,
  RightBrace,

  BangEqual,
  Equal,
  EqualEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
};

struct Token {
  TokenType type;
  std::string_view lexeme;
  SourceLocation location;

  Token(TokenType type, std::string_view lexeme, SourceLocation location)
    : type(type)
    , lexeme(lexeme)
    , location(location) {}
};
