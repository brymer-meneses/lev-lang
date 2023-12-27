#pragma once
#include <lev/diagnostics/sourceLocation.h>

namespace lev {

enum class TokenType {
  Integer,
  Float,
  String,
  True,
  False,

  Identifier,
  Let,
  Public,
  Mutable,
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

  Endline,
  Indent,
  Dedent,

  Return,

  End,
};

struct Token {
  TokenType type;
  std::string_view lexeme;
  SourceLocation location;

  constexpr Token(TokenType type, std::string_view lexeme,
                  SourceLocation location)
      : type(type), lexeme(lexeme), location(location) {}

  constexpr auto operator==(const Token& t) const -> bool {
    return this->type == t.type and this->lexeme == t.lexeme;
  }

  static constexpr auto typeToString(TokenType type) -> std::string_view {
    switch (type) {
    case TokenType::LeftParen:
      return "(";
    case TokenType::Endline:
      return "\\n";
    case TokenType::RightParen:
      return ")";
    case TokenType::LeftBracket:
      return "[";
    case TokenType::RightBracket:
      return "]";
    case TokenType::LeftBrace:
      return "{";
    case TokenType::RightBrace:
      return "}";
    case TokenType::Comma:
      return ",";
    case TokenType::Semicolon:
      return ";";
    case TokenType::Colon:
      return ":";
    case TokenType::Plus:
      return "+";
    case TokenType::PlusEqual:
      return "+=";
    case TokenType::Star:
      return "*";
    case TokenType::StarEqual:
      return "*=";
    case TokenType::Slash:
      return "/";
    case TokenType::SlashEqual:
      return "/=";
    case TokenType::Minus:
      return "-";
    case TokenType::MinusEqual:
      return "-=";
    case TokenType::Bang:
      return "!";
    case TokenType::BangEqual:
      return "!=";
    case TokenType::Greater:
      return ">";
    case TokenType::GreaterEqual:
      return ">=";
    case TokenType::RightArrow:
      return "->";
    case TokenType::Less:
      return "<";
    case TokenType::LessEqual:
      return "<=";
    case TokenType::Equal:
      return "=";
    case TokenType::EqualEqual:
      return "==";
    case TokenType::True:
      return "true";
    case TokenType::False:
      return "false";
    case TokenType::Indent:
      return "indent";
    case TokenType::Impl:
      return "impl";
    case TokenType::Dedent:
      return "dedent";
    case TokenType::End:
      return "eof";
    case TokenType::Identifier:
      return "identifier";
    case TokenType::Mutable:
      return "mutable";
    case TokenType::String:
      return "string";
    case TokenType::Integer:
      return "integer";
    case TokenType::Float:
      return "float";
    case TokenType::Return:
      return "return";
    case TokenType::Or:
      return "return";
    case TokenType::For:
      return "for";
    case TokenType::While:
      return "while";
    case TokenType::If:
      return "if";
    case TokenType::Else:
      return "else";
    case TokenType::Let:
      return "let";
    case TokenType::Break:
      return "break";
    case TokenType::Class:
      return "class";
    case TokenType::Not:
      return "not";
    case TokenType::And:
      return "and";
    case TokenType::Function:
      return "fn";
    case TokenType::Public:
      return "pub";
    }
  };
};

} // namespace lev
