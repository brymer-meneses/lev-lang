#include "parser.h"
#include "scanner.h"
#include <iostream>
#include <memory>
#include <ranges>

using namespace lev::parser;
using namespace lev::scanner;
using namespace lev::token;

Parser::Parser(std::string_view source) {
  Scanner scanner(source);
  auto tokens = scanner.scan();
  if (not tokens.has_value()) {
    Scanner::printError(tokens.error());
    return;
  }
  mTokens = std::move(tokens.value());
};

Parser::Parser(std::vector<Token> tokens) : mTokens(std::move(tokens)) {};

auto Parser::isAtEnd() const -> bool {
  return mCurrent >= mTokens.size();
}

auto Parser::peek() const -> std::optional<Token> {
  if (isAtEnd()) return std::nullopt;
  return mTokens[mCurrent];
}

auto Parser::peekPrev() const -> std::optional<Token> {
  if (mCurrent == 0) return std::nullopt;
  return mTokens[mCurrent - 1];
}

auto Parser::match(TokenType type) -> bool {
  if (isAtEnd()) return false;
  if (type == peek()->type) {
    mCurrent += 1;
    return true;
  }
  return false;
}

auto Parser::expect(TokenType type) -> std::optional<Token> {
  if (isAtEnd()) return std::nullopt;

  if (type == peek()->type) {
    mCurrent += 1;
    return peek().value();
  } else {
    return std::nullopt;
  }
}

auto Parser::parse() -> std::expected<std::vector<std::unique_ptr<Stmt>>, ParserError> {
  std::vector<std::unique_ptr<Stmt>> statements;
  while (not isAtEnd()) {
    auto stmt = parseDeclaration();
    if (not stmt) {
      return std::unexpected(stmt.error());
    }
    statements.push_back(std::move(stmt.value()));
  }
  return std::move(statements);
}


auto Parser::parseDeclaration() -> std::expected<std::unique_ptr<Stmt>, ParserError> {
  if (match(TokenType::Function)) {
    return parseFunctionDeclaration();
  }
  if (match(TokenType::Let)) {
    return parseVariableDeclaration();
  }
  return parseStmt();
}

static constexpr auto parseType(std::string_view tokenLexeme) -> Type {

  static constexpr auto lexemes = {
    "i8",
    "i16",
    "i32",
    "i64",
    "u8",
    "u16",
    "u32",
    "u64",
    "f32",
    "f64",
  };

  static constexpr auto types = {
    Type::i8,
    Type::i16,
    Type::i32,
    Type::i64,
    Type::u8,
    Type::u16,
    Type::u32,
    Type::u64,
    Type::f32,
    Type::f64,
  };

  for (auto [lexeme, type] : std::views::zip(lexemes, types)) {
    if (tokenLexeme == lexeme) {
      return type;
    }
  }

  return Type::UserDefined;
}

auto Parser::parseVariableDeclaration() -> std::expected<std::unique_ptr<Stmt>, ParserError> {
  bool isMutable = false;
  if (match(TokenType::Mutable)) {
    isMutable = true;
  }

  auto identifier = expect(TokenType::Identifier);
  if (not identifier) {
    return std::unexpected(UnexpectedToken(TokenType::Identifier, peek()->type));
  }

  Type type = Type::UserDefined;
  if (match(TokenType::Colon)) {
    auto identifier = expect(TokenType::Identifier);
    if (not identifier) {
      return std::unexpected(UnexpectedToken(TokenType::Equal, peek()->type));
    }
    type = parseType(identifier->lexeme);
  }

  if (not match(TokenType::Equal)) {
    return std::unexpected(UnexpectedToken(TokenType::Equal, peek()->type));
  }

  auto expr = parseExpr();
  if (not expr) {
    return std::unexpected(expr.error());
  }

  return std::make_unique<VariableDeclaration>(identifier.value(), isMutable, type, std::move(expr.value()));
}


auto Parser::parseFunctionDeclaration() -> std::expected<std::unique_ptr<Stmt>, ParserError> {

}

auto Parser::parseStmt() -> std::expected<std::unique_ptr<Stmt>, ParserError> {

}

auto Parser::parseExpr() -> std::expected<std::unique_ptr<Expr>, ParserError> {
  return parsePrimaryExpr();
}

auto Parser::parsePrimaryExpr() -> std::expected<std::unique_ptr<Expr>, ParserError> {
  if (match(TokenType::Float) || match(TokenType::Integer)) {
    auto token = peekPrev().value();
    return std::make_unique<LiteralExpr>(token);
  }
}

