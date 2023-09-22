#pragma once
#include <string>

namespace lev::token {

  enum class TokenType {
    LeftParen,
    RightParen,
    RightArrow,
    Function,
    Public,
    Identifier,
    Colon,
    Comma,

    Newline,
    Indent,
    Dedent,

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

  constexpr auto tokenTypeToString(TokenType type) -> std::string_view {
    switch (type) {
      case TokenType::LeftParen:
        return "(";
      case TokenType::RightParen:
        return ")";
      case TokenType::Comma:
        return ",";
      case TokenType::Newline:
        return "newline";
      case TokenType::Indent:
        return "indent";
      case TokenType::Dedent:
        return "dedent";
      case TokenType::Colon:
        return ":";
      case TokenType::EndOfFile:
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
      case TokenType::RightArrow:
        return "->";
      case TokenType::Function:
        return "fn";
      case TokenType::Public:
        return "pub";
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
        return "!";
      case TokenType::GreaterEqual:
        return "!=";
      case TokenType::Less:
        return "<";
      case TokenType::LessEqual:
        return "<=";
      case TokenType::Equal:
        return "=";
      case TokenType::EqualEqual:
        return "==";
    }
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
