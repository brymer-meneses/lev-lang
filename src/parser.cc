#include "parser.h"
#include "scanner.h"
#include <iostream>
#include <memory>
#include <ranges>
#include <format>
#include <map>

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

auto Parser::advance() -> std::optional<Token> {
  if (isAtEnd()) {
    return std::nullopt;
  }
  return mTokens[mCurrent++];
}

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
    return mTokens[mCurrent++];
  } else {
    return std::nullopt;
  }
}

auto Parser::parse() -> std::expected<std::vector<std::unique_ptr<Stmt>>, ParserError> {
  std::vector<std::unique_ptr<Stmt>> statements;
  while (not isAtEnd()) {
    while (match(TokenType::Newline)) {
      continue;
    }
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


auto Parser::parseType() -> Type {

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
    if (peek()->lexeme == lexeme) {
      advance();
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

  if (not match(TokenType::Colon)) {
    return std::unexpected(UnexpectedToken(TokenType::Colon, peekPrev()->type));
  }

  auto type = parseType();

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
  auto identifier = expect(TokenType::Identifier);
  if (not identifier) {
    return std::unexpected(UnexpectedToken(TokenType::Identifier, peek()->type));
  }

  auto args = std::vector<FunctionArg>{};
  if (not match(TokenType::LeftParen)) {
    return std::unexpected(UnexpectedToken(TokenType::LeftParen, peek()->type));
  }

  while (not match(TokenType::RightParen)) {
    auto argIdentifier = expect(TokenType::Identifier);
    if (not argIdentifier) {
      return std::unexpected(UnexpectedToken(TokenType::Identifier, peek()->type));
    }

    if (not match(TokenType::Colon)) {
      return std::unexpected(UnexpectedToken(TokenType::Colon, peek()->type));
    }

    auto type = parseType();

    args.push_back({argIdentifier.value().lexeme, type});

    if (not match(TokenType::Comma)) {
      if (not match(TokenType::RightParen)) {
        return std::unexpected(UnexpectedToken(TokenType::RightParen, peek()->type));
      }
      break;
    }
  }

  if (not match(TokenType::RightArrow)) {
    return std::unexpected(UnexpectedToken(TokenType::RightArrow, peek()->type));
  }

  auto returnType = parseType();

  if (not match(TokenType::Colon)) {
    return std::unexpected(UnexpectedToken(TokenType::Colon, peek()->type));
  }

  if (not match(TokenType::Indent)) {
    return std::unexpected(UnexpectedToken(TokenType::Indent, peek()->type));
  }

  auto body = parseBlock();

  if (not body) {
    return std::unexpected(body.error());
  }

  return std::make_unique<FunctionDeclaration>(identifier.value().lexeme, args, std::move(body.value()), returnType);
}

auto Parser::parseBlock() -> std::expected<std::vector<std::unique_ptr<Stmt>>, ParserError> {
  auto statements = std::vector<std::unique_ptr<Stmt>>{};
  while (not match(TokenType::Dedent)) {
    while (match(TokenType::Newline)) {
      continue;
    }
    auto stmt = parseDeclaration();
    if (not stmt) {
      return std::unexpected(stmt.error());
    }
    statements.push_back(std::move(stmt.value()));
  }
  return statements;
}

auto Parser::parseStmt() -> std::expected<std::unique_ptr<Stmt>, ParserError> {
  
  return std::unexpected(UnexpectedToken(TokenType::EqualEqual, peek()->type));
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

auto Parser::printError(ParserError error) -> void {
  struct ErrorVistor {
    auto operator()(const UnexpectedToken& err) const -> void {
      std::cerr << std::format("ERROR: Got an unexpected token `{}` expected `{}`\n", tokenTypeToString(err.found), tokenTypeToString(err.required));
    }
  };

  static ErrorVistor visitor;
  std::visit(visitor, error);
}

