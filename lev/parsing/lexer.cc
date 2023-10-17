
#include <ctype.h>
#include <map>
#include <print>

#include <lev/parsing/lexer.h>

using namespace lev;

auto Lexer::lex() -> std::expected<std::vector<Token>, LexingError>{
  std::vector<Token> tokens;

  while (not isAtEnd()) {
    auto token = lexNextToken();
    if (not token) {
      return std::unexpected(token.error());
    }
    tokens.push_back(*token);
  }

  tokens.emplace_back(Token(TokenType::End, "", getCurrentLocation()));
  return tokens;
}

auto Lexer::lexNextToken() -> std::expected<Token, LexingError> {
  auto c = advance();

  while (c == ' ') {
    mStart = mCurrent;
    c = advance();
  }

  switch (c) {
    case '"':
      return lexString();

    case '\n':
      mLastNewline = mCurrent - 1;
      mLine += 1;
      break;

    case '+': 
      if (match('=')) {
        return buildToken(TokenType::PlusEqual);
      } else {
        return buildToken(TokenType::Plus);
      }

    case '*': 
      if (match('=')) {
        return buildToken(TokenType::StarEqual);
      } else {
        return buildToken(TokenType::Star);
      }

    case '=':
      if (match('=')) {
        return buildToken(TokenType::EqualEqual);
      } else {
        return buildToken(TokenType::Equal);
      }

    case '-':
      if (match('>')){
        return buildToken(TokenType::RightArrow);
      } else if (match('=')) {
        return buildToken(TokenType::MinusEqual);
      } else {
        return buildToken(TokenType::Minus);
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

    case '/':
      if (match('=')) {
        return buildToken(TokenType::SlashEqual);
      } else {
        return buildToken(TokenType::Slash);
      }

    default:
      if (std::isdigit(c)) {
        return lexNumber();
      } else if (std::isalnum(c) or c == '_') {
        return lexIdentifier();
      } else {
        return std::unexpected(LexingError::UnexpectedCharacter(c, getPrevCharLocation()));
      }
  }

  __builtin_unreachable();
}

auto Lexer::lexNumber() -> std::expected<Token, LexingError> {
  bool didVisitPoint = false;

  if (peekPrev() == '.') {
    didVisitPoint = true;
  }

  while (std::isdigit(peek()) or peek() == '.') {
    if (peek() == '.') {
      if (didVisitPoint) {
        return std::unexpected(LexingError::RedundantDecimalPoint(getCurrentLocation()));
      }
      didVisitPoint = true;
    }
    advance();
  }

  return buildToken(TokenType::Number);
}

auto Lexer::lexIdentifier() -> std::expected<Token, LexingError> {
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
    return buildToken(map.at(identifierStr));
  } else {
    return buildToken(TokenType::Identifier); 
  }

}

auto Lexer::lexString() -> std::expected<Token, LexingError> {
  while (peek() != '"') {
    if (isAtEnd()) {
      return std::unexpected(LexingError::UnterminatedString(getPrevCharLocation()));
    }
    advance();
  }

  // consume the last '"'
  advance();

  return buildToken(TokenType::String);
}

auto Lexer::buildToken(TokenType type) -> Token {
  auto token = Token(type, mSource.substr(mStart, mCurrent - mStart), getCurrentLocation());
  mStart = mCurrent;
  return token;
}

auto Lexer::getCurrentLocation() -> SourceLocation {
  return SourceLocation(mFilename, mLastNewline - mStart, mLastNewline - mCurrent, mLine);
}

auto Lexer::getCurrentCharLocation() -> SourceLocation {
  auto pos = mLastNewline - mCurrent;
  return SourceLocation(mFilename, pos, pos, mLine);
}

auto Lexer::getPrevCharLocation() -> SourceLocation {
  auto pos = mLastNewline - mCurrent - 1;
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

auto Lexer::peek() const -> char {
  if (isAtEnd()) return '\0';
  return mSource.at(mCurrent);
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
  mLastNewline = 0;
}
