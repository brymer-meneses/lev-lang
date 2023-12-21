#pragma once
#include "../sourceLocation.h"

namespace lev {

enum class TokenType {
  Number,
  String,
  Boolean,

  Identifier,
  Let,

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

  Colon,
  Semicolon,
  Comma,

  End,
};

struct Token {
  TokenType type;
  std::string_view lexeme;
  SourceLocation location;

  constexpr Token(TokenType type, std::string_view lexeme, SourceLocation location)
    : type(type)
    , lexeme(lexeme)
    , location(location) {}
};

}
