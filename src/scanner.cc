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
  mStart = mCurrent;
  return Token(type, substr);
}

auto Scanner::scanNumber() -> std::expected<Token, ScannerError> {

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

  return buildToken(decimalPoints == 0 ? TokenType::Integer : TokenType::Float);
}

auto Scanner::scanIdentifier() -> std::expected<Token, ScannerError> {
  while (std::isalnum(peek()) or peek() == '_') {
    advance();
  }

  static constexpr auto getIdentifier = [](std::string_view lexeme) -> TokenType {
    if (lexeme == "fn") {
      return TokenType::Function;
    } else if (lexeme == "pub") {
      return TokenType::Public;
    } else if (lexeme == "return") {
      return TokenType::Return;
    } else if (lexeme == "for") {
      return TokenType::For;
    } else if (lexeme == "while") {
      return TokenType::While;
    } else if (lexeme == "if") {
      return TokenType::If;
    } else if (lexeme == "else") {
      return TokenType::Else;
    } else if (lexeme == "let") {
      return TokenType::Let;
    } else if (lexeme == "mut") {
      return TokenType::Mutable;
    }

    return TokenType::Identifier;
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
  return buildToken(TokenType::String);
}


auto Scanner::getNextToken() -> std::expected<Token, ScannerError> {

  auto c = advance();
 
  while (c == ' ' or c == '\r') {
    mStart = mCurrent;
    c = advance();
  }

  switch (c) {
    case '\0':
      return buildToken(TokenType::EndOfFile);
    case ':':
      return buildToken(TokenType::Colon);
    case '"':
      return scanString();
    case '+':
      if (match('=')) {
        return buildToken(TokenType::PlusEqual);
      } else {
        return buildToken(TokenType::Plus);
      }
    case '-':
      if (match('>')) {
        return buildToken(TokenType::RightArrow);
      } else if (match('=')) {
        return buildToken(TokenType::MinusEqual);
      } else {
        return buildToken(TokenType::Minus);
      }
    case '*':
      if (match('=')) {
        return buildToken(TokenType::StarEqual);
      } else {
        return buildToken(TokenType::Star);
      }
    case '/':
      if (match('=')) {
        return buildToken(TokenType::SlashEqual);
      } else {
        return buildToken(TokenType::Slash);
      }
    case '=':
      if (match('=')) {
        return buildToken(TokenType::EqualEqual);
      } else {
        return buildToken(TokenType::Equal);
      }
    case '>':
      if (match('=')) {
        return buildToken(TokenType::GreaterEqual);
      } else {
        return buildToken(TokenType::Greater);
      }
    case '<':
      if (match('=')) {
        return buildToken(TokenType::LessEqual);
      } else {
        return buildToken(TokenType::Less);
      }
    case '!':
      if (match('=')) {
        return buildToken(TokenType::BangEqual);
      } else {
        return buildToken(TokenType::Bang);
      }
    default:
      if (std::isdigit(c) or c == '.') {
        return scanNumber();
      } else if (std::isalnum(c) or c == '_') {
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
