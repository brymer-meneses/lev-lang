#include <lev/parsing/parser.h>
#include <lev/utils.h>

using namespace lev;

auto Parser::parse() -> std::expected<std::vector<Stmt>, ParsingError> {
  std::vector<Stmt> statements;
  while (not isAtEnd()) {
    auto statement = parseDeclaration();
    if (not statement) {
      return std::unexpected(statement.error());
    }
    statements.push_back(std::move(*statement));
  }
  return statements;
}

auto Parser::parseDeclaration() -> std::expected<Stmt, ParsingError> {
  if (match(TokenType::Function)) {
    return parseFunctionDeclaration();
  }

  if (match(TokenType::Let)) {
    return parseVariableDeclaration();
  }

  return parseStatement();
}

auto Parser::parseStatement() -> std::expected<Stmt, ParsingError> {
  return std::unexpected(ParsingError::UnexpectedToken(peek().type, getCurrentLocation()));
}

auto Parser::parseExpression() -> std::expected<Expr, ParsingError> {
  return parseLiteralExpr();
}

auto Parser::parseLiteralExpr() -> std::expected<Expr, ParsingError> {
  if (match({TokenType::Number, TokenType::String, TokenType::Identifier, TokenType::Boolean})) {
    return LiteralExpr(peekPrev());
  }

  return std::unexpected(ParsingError::UnexpectedToken(peek().type, getCurrentLocation()));
}

auto Parser::getCurrentLocation() -> SourceLocation {
  return peek().location;
}

auto Parser::setTokens(std::vector<Token> tokens) -> void {
  mTokens = std::move(tokens);
}

auto Parser::match(TokenType type) -> bool {
  if (peek().type == type) {
    mCurrent += 1;
    return true;
  }
  return false;
}

auto Parser::match(std::initializer_list<TokenType> types) -> bool {
  for (const auto& type : types) {
    if (peek().type == type) {
      mCurrent += 1;
      return true;
    }
  }
  return false;
}

auto Parser::isAtEnd() const -> bool {
  return mCurrent >= mTokens.size();
}

auto Parser::peek() const -> const Token& {
  if (isAtEnd()) {
    return mTokens.back();
  }
  return mTokens[mCurrent];
}

auto Parser::peekPrev() const -> const Token& {
  if (mCurrent == 0) {
    RAISE_INTERNAL_ERROR("Tried to call `peekPrev` when mCurrent is 0");
  }
  return mTokens[mCurrent];
}

