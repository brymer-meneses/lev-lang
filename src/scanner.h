#pragma once

#include <expected>
#include <string_view>
#include <string>
#include <variant>
#include <vector>
#include <stack>

#include "token.h"

namespace lev::scanner {
  using namespace lev::token;

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

  struct UnterminatedString {
    std::string_view lexeme;
    UnterminatedString(std::string_view lexeme): lexeme(lexeme) {};
  };

  using ScannerError = std::variant<UnexpectedCharacter, InvalidNumber, InvalidString, UnterminatedString>;

  class Scanner {
    std::string_view mSource;
    size_t mCurrent = 0;
    size_t mStart = 0;
    std::stack<size_t> mIndentationStack;

    public:
      Scanner(std::string_view source);
      auto scan() -> std::expected<std::vector<Token>, ScannerError>;
    
      static auto printError(ScannerError error) -> void;

    private:
      auto getNextToken() -> std::expected<Token, ScannerError>;

      auto advance() -> char;
      auto match(char c) -> bool;

      auto buildToken(TokenType type) -> std::expected<Token, ScannerError>;

      auto scanNumber() -> std::expected<Token, ScannerError>;
      auto scanString() -> std::expected<Token, ScannerError>;
      auto scanIdentifier() -> std::expected<Token, ScannerError>;
      auto scanIndentation() -> std::expected<Token, ScannerError>;

      auto peek() const -> char;
      auto isAtEnd() const -> bool;
  };


}
