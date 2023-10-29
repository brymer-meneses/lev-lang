#include <lev/parsing/parser.h>
#include <lev/utils.h>
#include <map>

using namespace lev;

#define CONSUME(tokenType)                                                     \
  {                                                                            \
    auto tmp = expect(tokenType);                                              \
    if (not tmp) {                                                             \
      return std::unexpected(tmp.error());                                     \
    }                                                                          \
  }

auto Parser::parse() -> std::expected<std::vector<Stmt>, ParsingError> {
  std::vector<Stmt> statements;
  while (not isAtEnd()) {
    auto statement = parseDeclaration();
    if (not statement) {
      return std::unexpected(statement.error());
    }
    statements.push_back(std::move(*statement));
  };
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

auto Parser::parseFunctionDeclaration() -> std::expected<Stmt, ParsingError> {
  return std::unexpected(ParsingError::Unimplemented());
}


auto Parser::parseVariableDeclaration() -> std::expected<Stmt, ParsingError> {
  auto identifier = expect(TokenType::Identifier);
  if (not identifier) {
    return std::unexpected(identifier.error());
  }

  std::expected<LevType, ParsingError> type = LevType::Inferred();

  if (match(TokenType::Colon)) {
    type = parseType();
    if (not type) {
      return std::unexpected(type.error());
    }
  } 

  CONSUME(TokenType::Equal);

  auto value = parseExpression();

  return Stmt::VariableDeclaration(*identifier, *type, std::move(*value));
}

auto Parser::parseType() -> std::expected<LevType, ParsingError> {

  static const std::map<std::string_view, LevType::Builtin> builtinTypes = {
    {"i8", LevType::Builtin::i8() },
    {"i16", LevType::Builtin::i16() },
    {"i32", LevType::Builtin::i32() },
    {"i64", LevType::Builtin::i64() },

    {"u8", LevType::Builtin::u8() },
    {"u16", LevType::Builtin::u16() },
    {"u32", LevType::Builtin::u32() },
    {"u64", LevType::Builtin::u64() },

    {"f32", LevType::Builtin::f32() },
    {"f64", LevType::Builtin::f64() },
  };

  auto type = expect(TokenType::Identifier);
  if (not type) {
    return std::unexpected(type.error());
  }

  if (builtinTypes.contains(type->lexeme)) {
    return builtinTypes.at(type->lexeme);
  } 

  return LevType::UserDefined(*type);
}

auto Parser::parseStatement() -> std::expected<Stmt, ParsingError> {
  return std::unexpected(ParsingError::Unimplemented());
}

auto Parser::parseExpression() -> std::expected<Expr, ParsingError> {
  return parseLiteralExpr();
}

auto Parser::parseLiteralExpr() -> std::expected<Expr, ParsingError> {
  if (match({TokenType::Number, TokenType::String, TokenType::Identifier, TokenType::Boolean})) {
    return Expr::Literal(peekPrev());
  }

  return std::unexpected(ParsingError::Unimplemented());
}

auto Parser::getCurrentLocation() -> SourceLocation {
  return peek().location;
}

auto Parser::setTokens(std::vector<Token> tokens) -> void {
  mTokens = std::move(tokens);
}

auto Parser::expect(TokenType type) -> std::expected<Token, ParsingError> {
  if (match(type)) {
    return peekPrev();
  }
  return std::unexpected(ParsingError::UnexpectedToken(type, peek().type, getCurrentLocation()));
}

auto Parser::match(TokenType type) -> bool {
  if (peek().type == type) {
    mCurrent += 1;
    return true;
  }
  return false;
}

auto Parser::check(TokenType type) const -> bool {
  if (peek().type == type) {
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
  return peek().type == TokenType::End;
}

auto Parser::peek() const -> const Token& {
  if (mCurrent >= mTokens.size()) {
    return mTokens.back();
  }

  return mTokens[mCurrent];
}

auto Parser::peekPrev() const -> const Token& {
  if (mCurrent == 0) {
    RAISE_INTERNAL_ERROR("Tried to call `peekPrev` when mCurrent is 0");
  }
  return mTokens[mCurrent-1];
}

