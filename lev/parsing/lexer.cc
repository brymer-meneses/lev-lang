
#include <ctype.h>
#include <map>
#include <print>

#include <lev/parsing/lexer.h>

using namespace lev;

auto Lexer::lex() -> std::expected<std::vector<Token>, LexError>{

  while (not isAtEnd()) {
    mStart = mCurrent;
    auto token = lexNextToken();
    if (not token) {
      return std::unexpected(token.error());
    }
  }

  mTokens.emplace_back(Token(TokenType::End, "", getCurrentLocation()));
  return mTokens;
}


auto Lexer::lexNextToken() -> std::expected<void, LexError> {
  auto c = advance();


  switch (c) {
    case '"':
      lexString();
      break;

    case ',':
      buildToken(TokenType::Comma);
      break;

    case ';':
      buildToken(TokenType::Semicolon);
      break;

    case '\n':
      mLineStart = mCurrent;
      mLine += 1;
      break;

    case ':':
      buildToken(TokenType::Colon);
      break;

    case '+': 
      if (match('=')) {
        buildToken(TokenType::PlusEqual);
      } else {
        buildToken(TokenType::Plus);
      }
      break;

    case '*': 
      if (match('=')) {
        buildToken(TokenType::StarEqual);
      } else {
        buildToken(TokenType::Star);
      }
      break;

    case '=':
      if (match('=')) {
        buildToken(TokenType::EqualEqual);
      } else {
        buildToken(TokenType::Equal);
      }
      break;

    case '-':
      if (match('>')){
        buildToken(TokenType::RightArrow);
      } else if (match('=')) {
        buildToken(TokenType::MinusEqual);
      } else {
        buildToken(TokenType::Minus);
      }
      break;

    case '>':
      if (match('=')) {
        buildToken(TokenType::GreaterEqual);
      } else {
        buildToken(TokenType::Greater);
      }
      break;

    case '<':
      if (match('=')) {
        buildToken(TokenType::LessEqual);
      } else {
        buildToken(TokenType::Less);
      }
      break;

    case '!':
      if (match('=')) {
        buildToken(TokenType::BangEqual);
      } else {
        buildToken(TokenType::Bang);
      }
      break;

    case '/':
      if (match('/')) {
        while (peek() != '\n' and not isAtEnd()) {
          advance();
        }
      } else if (match('=')) {
        buildToken(TokenType::SlashEqual);
      } else {
        buildToken(TokenType::Slash);
      }
      break;

    default:
      if (std::isdigit(c)) {
        lexNumber();
      } else if (std::isalnum(c) or c == '_') {
        lexIdentifier();
      } else {
        std::unexpected(LexError::UnexpectedCharacter(c, getPrevCharLocation()));
      }
  }
  return {};
}

auto Lexer::lexNumber() -> std::expected<void, LexError> {
  bool didVisitPoint = false;

  if (peekPrev() == '.') {
    didVisitPoint = true;
  }

  while (std::isdigit(peek()) or peek() == '.') {
    if (peek() == '.') {
      if (didVisitPoint) {
        return std::unexpected(LexError::RedundantDecimalPoint(getCurrentLocation()));
      }
      didVisitPoint = true;
    }
    advance();
  }

  buildToken(TokenType::Number);
  return {};
}

auto Lexer::lexIdentifier() -> std::expected<void, LexError> {
  static const std::map<std::string_view, TokenType> map = {
    {"fn", TokenType::Function},
    {"for", TokenType::For},
    {"break", TokenType::Break},
    {"while", TokenType::While},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"mut", TokenType::Mut},
    {"and", TokenType::And},
    {"or", TokenType::Or},
    {"impl", TokenType::Impl},
    {"class", TokenType::Class},
    {"not", TokenType::Not},
    {"or", TokenType::Or},
    {"and", TokenType::And},
    {"let", TokenType::Let}
  };

  while (std::isalnum(peek()) or peek() == '_') {
    advance();
  }

  auto identifierStr = mSource.substr(mStart, mCurrent - mStart);
  if (map.contains(identifierStr)) {
    buildToken(map.at(identifierStr));
  } else {
    buildToken(TokenType::Identifier); 
  }

  return {};
}

auto Lexer::lexString() -> std::expected<void, LexError> {
  while (peek() != '"') {
    if (isAtEnd()) {
      return std::unexpected(LexError::UnterminatedString(getPrevCharLocation()));
    }
    advance();
  }

  // consume the last '"'
  advance();
  buildToken(TokenType::String);

  return {};
}

auto Lexer::buildToken(TokenType type) -> void {
  auto token = Token(type, mSource.substr(mStart, mCurrent - mStart), getCurrentLocation());
  mTokens.push_back(token);
}

auto Lexer::getCurrentLocation() -> SourceLocation {
  return SourceLocation(mFilename, mLineStart - mStart, mLineStart - mCurrent, mLine);
}

auto Lexer::getCurrentCharLocation() -> SourceLocation {
  auto pos = mLineStart - mCurrent;
  return SourceLocation(mFilename, pos, pos, mLine);
}

auto Lexer::getPrevCharLocation() -> SourceLocation {
  auto pos = mLineStart - mCurrent - 1;
  return SourceLocation(mFilename, pos, pos, mLine);
}

auto Lexer::advance() -> char {
  if (isAtEnd()) {
    return '\0';
  };

  mCurrent += 1;
  return mSource.at(mCurrent-1);
}

auto Lexer::match(const char expected) -> bool {
  if (peek() == expected) {
    mCurrent += 1;
    return true;
  }
  return false;
}

auto Lexer::match(std::initializer_list<const char> chars) -> bool {
  for (auto c : chars) {
    if (peek() == c) {
      mCurrent += 1;
      return true;
    }
  }
  return false;
}

auto Lexer::check(const char expected) const -> bool {
  if (peek() == expected) {
    return true;
  }
  return false;
}

auto Lexer::check(std::initializer_list<const char> chars) const -> bool {
  for (auto c : chars) {
    if (peek() == c) {
      return true;
    }
  }
  return false;
}

auto Lexer::peek() const -> char {
  if (isAtEnd()) return '\0';
  return mSource.at(mCurrent);
}

auto Lexer::peekNext() const -> char {
  if (mCurrent + 1 >= mSource.length()) 
    return '\0';

  return mSource.at(mCurrent + 1);
}

auto Lexer::peekPrev() const -> char {
  if (mCurrent == 0) return mSource.at(0);
  return mSource.at(mCurrent-1);
}

auto Lexer::isAtEnd() const -> bool {
  return mCurrent >= mSource.length();
}

auto Lexer::setSource(std::string_view source) -> void {
  mSource = source;
}

auto Lexer::setFilename(std::string_view filename) -> void {
  mFilename = filename;
}

auto Lexer::reset() -> void {
  mCurrent = 0;
  mSource = "";
  mFilename = "anonymous";
  mLine = 0;
  mLineStart = 0;
}
