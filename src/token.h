#pragma once
#include <string>

namespace lev::token {

  enum class TokenType {
    RightArrow,
    Function,
    Public,
    Identifier,
    Colon,

    Integer,
    Float,
    String,

    For,
    While,
    Return,
    If,
    Else,
    Let,
    Mutable,
    
    Bang,
    BangEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    Equal,
    EqualEqual,

    Plus,
    PlusEqual,
    Star,
    StarEqual,
    Minus,
    MinusEqual,
    Slash,
    SlashEqual,

    EndOfFile,
  };

  struct Token {
    TokenType type;
    std::string_view lexeme;
    Token(TokenType type, std::string_view lexeme) : type(type), lexeme(lexeme) {}
    Token() = default;

    auto operator==(const Token& t) const -> bool {
      return t.lexeme == lexeme and t.type == type;
    }
  };


}
