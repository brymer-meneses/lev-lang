#include <cctype>
#include "scanner.h"

using namespace lev::scanner;

Scanner::Scanner(const std::string& source) : mSource(source) {};

auto Scanner::advance() -> char {
  if (isAtEnd()) {
    return '\0';
  }
  return mSource[mCurrent++];
}

auto Scanner::peek() const -> char {
  if (isAtEnd()) {
    return '\0';
  }
  return mSource[mCurrent];
}

auto Scanner::isAtEnd() const -> bool {
  return mCurrent >= mSource.length();
}

auto Scanner::match(char c) -> bool {
  if (c == peek()) {
    mCurrent += 1;
    return true;
  }
  return false;
}

auto Scanner::buildToken(TokenType type) -> std::expected<Token, ScannerError> {
  auto t = Token(type, mSource.substr(mStart, mStart - mCurrent));
  mStart = mCurrent;
  return t;
}

auto Scanner::scanNumber() -> std::expected<Token, ScannerError> {
  using T = TokenType;
  auto c = advance();
  bool isInteger = true;
  while (std::isdigit(c) or c == '.') {
    if (c == '.') {
      if (isInteger) {
        isInteger = false;
      } else {
        // we already found a dot, and another dot appeared again?
        auto substr = mSource.substr(mStart, mStart - mCurrent);
        return std::unexpected(InvalidNumber(substr));
      }
    }

    c = advance();
  }
  return buildToken(isInteger ? T::Integer : T::Float);
}

auto Scanner::scanIdentifier() -> std::expected<Token, ScannerError> {
  while (std::isalnum(advance())) {
    mCurrent += 1;
  }

  static constexpr auto getIdentifier = [](std::string_view lexeme) -> TokenType {
    using T = TokenType;
    if (lexeme == "fn") {
      return T::Function;
    } else if (lexeme == "pub") {
      return T::Public;
    } else if (lexeme == "return") {
      return T::Return;
    } else if (lexeme == "for") {
      return T::For;
    } else if (lexeme == "while") {
      return T::While;
    }
    return T::Identifier;
  };

  auto lexeme = mSource.substr(mStart, mStart-mCurrent);
  return buildToken(getIdentifier(lexeme));
}


auto Scanner::getNextToken() -> std::expected<Token, ScannerError> {
  using T = TokenType;
  auto c = advance();
 
  while (c == ' ' or c == '\r') {
    c = advance();
  }

  switch (c) {
    case '\0':
      return buildToken(T::EndOfFile);
    case '+':
      if (match('=')) {
        return buildToken(T::PlusEqual);
      } else {
        return buildToken(T::Plus);
      }
    case '-':
      if (match('>')) {
        return buildToken(T::RightArrow);
      } else if (match('=')) {
        return buildToken(T::MinusEqual);
      } else {
        return buildToken(T::Minus);
      }
    case '*':
      if (match('=')) {
        return buildToken(T::StarEqual);
      } else {
        return buildToken(T::Star);
      }
    case '/':
      if (match('=')) {
        return buildToken(T::SlashEqual);
      } else {
        return buildToken(T::Slash);
      }
    case '=':
      if (match('=')) {
        return buildToken(T::EqualEqual);
      } else {
        return buildToken(T::Equal);
      }
    case '>':
      if (match('=')) {
        return buildToken(T::GreaterEqual);
      } else {
        return buildToken(T::Greater);
      }
    case '<':
      if (match('=')) {
        return buildToken(T::LessEqual);
      } else {
        return buildToken(T::Less);
      }
    case '!':
      if (match('=')) {
        return buildToken(T::BangEqual);
      } else {
        return buildToken(T::Bang);
      }
    default:
      if (std::isdigit(c)) {
        return scanNumber();
      } else if (std::isalnum(c)) {
        return scanIdentifier();
      } else {
        return std::unexpected(UnexpectedCharacter(c));
      }
  };
  __builtin_unreachable();
}

