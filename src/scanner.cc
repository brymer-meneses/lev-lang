#include <cctype>
#include <iostream>
#include "scanner.h"

using namespace lev::scanner;

Scanner::Scanner(std::string_view source) : mSource(source) {};

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
  auto substr = mSource.substr(mStart, mCurrent - mStart);
  auto t = Token(type, substr);

  mStart = mCurrent;
  return t;
}

auto Scanner::scanNumber() -> std::expected<Token, ScannerError> {
  using T = TokenType;

  int decimalPoints = 0;

  while (std::isdigit(peek()) or peek() == '.') {
    if (peek() == '.') {
      decimalPoints ++;
    }
    advance();
  }

  if (decimalPoints > 1) {
    return std::unexpected(InvalidNumber(mSource.substr(mStart, mCurrent - mStart)));
  }

  return buildToken(decimalPoints == 0 ? T::Integer : T::Float);
}

auto Scanner::scanIdentifier() -> std::expected<Token, ScannerError> {
  while (std::isalnum(peek())) {
    advance();
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

  auto lexeme = mSource.substr(mStart, mCurrent - mStart);
  return buildToken(getIdentifier(lexeme));
}

auto Scanner::scanString() -> std::expected<Token, ScannerError> {
  while (peek() != '"' and not isAtEnd()) {
    advance();
  }

  if (isAtEnd()) {
    return std::unexpected(UnterminatedString(mSource.substr(mStart, mCurrent - mStart)));
  }

  // consume the '"'
  advance();
  using T = TokenType;
  return buildToken(T::String);
}


auto Scanner::getNextToken() -> std::expected<Token, ScannerError> {

  using T = TokenType;
  auto c = advance();
 
  while (c == ' ' or c == '\r') {
    mStart = mCurrent;
    c = advance();
  }

  switch (c) {
    case '\0':
      return buildToken(T::EndOfFile);
    case ':':
      return buildToken(T::Colon);
    case '"':
      return scanString();
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

auto Scanner::scan() -> std::expected<std::vector<Token>, ScannerError> {
  std::vector<Token> tokens;
  while (not isAtEnd()) {
    auto tok = getNextToken();
    if (not tok) {
      return std::unexpected(tok.error());
    }
    tokens.push_back(tok.value());
  }
  return tokens;
}


auto Scanner::printError(ScannerError error) -> void {
  struct ErrorVistor {
    auto operator()(const UnexpectedCharacter& err) const -> void {
      std::cerr << "Got an unexpected character: " << err.character << "\n";
    }
    auto operator()(const InvalidNumber& err) const -> void {
      std::cerr << "Got an invalid number: " << err.lexeme << "\n";
    }
    auto operator()(const InvalidString& err) const -> void {
      std::cerr << "Got an invalid string: " << err.lexeme << "\n";
    }
    auto operator()(const UnterminatedString& err) const -> void {
      std::cerr << "Unterminated string: " << err.lexeme << "\n";
    }
  };

  static ErrorVistor visitor;
  std::visit(visitor, error);
}
