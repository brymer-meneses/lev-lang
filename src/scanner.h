#pragma once

#include <expected>
#include <string_view>
#include <string>
#include <variant>

#include "token.h"

namespace lev::scanner {

  struct UnexpectedCharacter {
    const char character;
    UnexpectedCharacter(char character): character(character) {};
  };

  struct InvalidNumber {
    std::string_view lexeme;
    InvalidNumber(std::string_view lexeme): lexeme(lexeme) {};
  };

  struct InvalidString {
    std::string_view lexeme;
    InvalidString(std::string_view lexeme): lexeme(lexeme) {};
  };

  using ScannerError = std::variant<UnexpectedCharacter, InvalidNumber, InvalidString>;

  class Scanner {
    const std::string& mSource;
    size_t mCurrent = 0;
    size_t mStart = 0;

    public:
      Scanner(const std::string& source);
      auto getNextToken() -> std::expected<Token, ScannerError>;

    private:
      auto advance() -> char;
      auto match(char c) -> bool;

      auto buildToken(TokenType type) -> std::expected<Token, ScannerError>;

      auto scanNumber() -> std::expected<Token, ScannerError>;
      auto scanIdentifier() -> std::expected<Token, ScannerError>;
      auto scanString() -> std::expected<Token, ScannerError>;

      auto peek() const -> char;
      auto isAtEnd() const -> bool;
  };


}
