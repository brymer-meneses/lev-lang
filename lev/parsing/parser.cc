#include <lev/parsing/parser.h>
#include <lev/misc/macros.h>
#include <lev/misc/match.h>

#include <map>

using namespace lev;

#define CONSUME(tokenType)                                                     \
  {                                                                            \
    auto tmp = expect(tokenType);                                              \
    if (not tmp) {                                                             \
      return std::unexpected(tmp.error());                                     \
    }                                                                          \
  }

auto Parser::parse() -> std::expected<std::vector<Stmt>, ParseError> {
  std::vector<Stmt> statements;
  while (not isAtEnd()) {
    auto statement = TRY(parseDeclaration());
    statements.push_back(std::move(statement));
  };
  return statements;
}

auto Parser::parseDeclaration() -> std::expected<Stmt, ParseError> {
  if (match(TokenType::Function)) {
    return parseFunctionDeclaration();
  }
  
  if (match(TokenType::Let)) {
    return parseVariableDeclaration();
  }

  return parseStatement();
}

auto Parser::parseFunctionArgument() -> std::expected<FunctionArgument, ParseError> {
  auto argName = TRY(expect(TokenType::Identifier));
  CONSUME(TokenType::Colon);
  auto type = TRY(parseType());
  return FunctionArgument(argName, type);
}

auto Parser::parseFunctionDeclaration() -> std::expected<Stmt, ParseError> {
  auto identifier = TRY(expect(TokenType::Identifier));
  CONSUME(TokenType::LeftParen);

  auto arguments = std::vector<FunctionArgument>{};

  while (not match(TokenType::RightParen)) {
    auto argument = TRY(parseFunctionArgument());

    if (not check(TokenType::RightParen)) {
      CONSUME(TokenType::Comma);
    }

    arguments.push_back(argument);
  }

  CONSUME(TokenType::RightArrow);
  auto returnType = TRY(parseType());

  CONSUME(TokenType::Colon);
  auto body = TRY(parseBlockStmt());
  return Stmt::FunctionDeclaration(identifier, std::move(arguments), returnType, std::move(body));
}

auto Parser::parseBlockStmt() -> std::expected<Stmt, ParseError> {
  CONSUME(TokenType::Indent);
  auto statements = std::vector<Stmt>{};
  while (not match(TokenType::Dedent) and not isAtEnd()) {
    auto stmt = parseStatement();
    if (not stmt) {
      return stmt;
    }
    statements.push_back(std::move(*stmt));
  }
  return Stmt::Block(std::move(statements));
}

auto Parser::parseReturnStmt() -> std::expected<Stmt, ParseError> {
  auto expr = TRY(parseExpression());
  return Stmt::Return(std::move(expr));
}

auto Parser::parseVariableDeclaration() -> std::expected<Stmt, ParseError> {
  auto identifier = TRY(expect(TokenType::Identifier));

  LevType type = LevType::Inferred();

  if (match(TokenType::Colon)) {
    type = TRY(parseType());
  } 

  CONSUME(TokenType::Equal);

  auto value = TRY(parseExpression());

  return Stmt::VariableDeclaration(identifier, type, std::move(value));
}

auto Parser::parseType() -> std::expected<LevType, ParseError> {

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

  auto type = TRY(expect(TokenType::Identifier));

  if (builtinTypes.contains(type.lexeme)) {
    return builtinTypes.at(type.lexeme);
  } 

  return LevType::UserDefined(type);
}

auto Parser::parseStatement() -> std::expected<Stmt, ParseError> {
  if (match(TokenType::Let)) {
    return parseVariableDeclaration();
  }

  if (match(TokenType::Return)) {
    return parseReturnStmt();
  }

  return std::unexpected(ParseError::Unimplemented());
}

auto Parser::parseExpression() -> std::expected<Expr, ParseError> {
  return parseLiteralExpr();
}

auto Parser::parseLiteralExpr() -> std::expected<Expr, ParseError> {
  if (match({TokenType::Integer, TokenType::String, TokenType::Identifier, TokenType::True, TokenType::False})) {
    return Expr::Literal(peekPrev());
  }

  return std::unexpected(ParseError::Unimplemented());
}

auto Parser::getCurrentLocation() -> SourceLocation {
  return peek().location;
}

auto Parser::setTokens(std::vector<Token> tokens) -> void {
  mTokens = std::move(tokens);
}

auto Parser::expect(TokenType expected) -> std::expected<Token, ParseError> {
  if (match(expected)) {
    return peekPrev();
  }
  return std::unexpected(ParseError::UnexpectedToken(peek().type, expected, getCurrentLocation()));
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

