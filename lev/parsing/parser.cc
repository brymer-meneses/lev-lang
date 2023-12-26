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

  if (match(TokenType::If)) {
    return parseControlStmt();
  }

  if (match(TokenType::Identifier)) {
    return parseAssignmentStmt();
  }

  return parseStatement();
}

auto Parser::parseAssignmentStmt() -> std::expected<Stmt, ParseError> {
  const auto identifier = peekPrev();
  CONSUME(TokenType::Equal);
  auto value = TRY(parseExpression());
  return Stmt::Assignment(identifier, std::move(value));
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
    if (check(TokenType::Endline)) {
      CONSUME(TokenType::Endline);
    }
    auto stmt = TRY(parseStatement());
    statements.push_back(std::move(stmt));
  }
  return Stmt::Block(std::move(statements));
}

auto Parser::parseReturnStmt() -> std::expected<Stmt, ParseError> {
  if (match(TokenType::Endline)) {
    return Stmt::Return();
  }
  auto expr = TRY(parseExpression());
  return Stmt::Return(std::move(expr));
}

auto Parser::parseControlStmt() -> std::expected<Stmt, ParseError> {
  auto condition = TRY(parseExpression());

  CONSUME(TokenType::Colon);
  auto body = TRY(parseBlockStmt());

  auto ifBranch = Branch(std::move(condition), std::move(body));
  std::vector<Branch> elseIfBranches;
  while (match(TokenType::Else) and match(TokenType::If)) {
    auto condition = TRY(parseExpression());
    CONSUME(TokenType::Colon)
    auto body = TRY(parseBlockStmt());
    elseIfBranches.push_back(Branch(std::move(condition), std::move(body)));
  }

  std::optional<Stmt> elseBody = std::nullopt;
  if (peekPrev().type == TokenType::Else) {
    CONSUME(TokenType::Colon);
    elseBody = TRY(parseBlockStmt());
  }

  return Stmt::Control(std::move(ifBranch), std::move(elseIfBranches), std::move(elseBody));
}

auto Parser::parseVariableDeclaration() -> std::expected<Stmt, ParseError> {
  auto isMutable = match(TokenType::Mutable);
  auto identifier = TRY(expect(TokenType::Identifier));

  LevType type = LevType::Inferred();

  if (match(TokenType::Colon)) {
    type = TRY(parseType());
  } 

  CONSUME(TokenType::Equal);

  auto value = TRY(parseExpression());

  return Stmt::VariableDeclaration(identifier, type, std::move(value), isMutable);
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

auto Parser::parseBinaryExprRHS(int exprPrec, Expr lhs) -> std::expected<Expr, ParseError> {
  static constexpr auto getTokenPrecedence = [](TokenType tokenType) -> int {
    switch (tokenType) {
      case TokenType::Less:
      case TokenType::LessEqual:
      case TokenType::Greater:
      case TokenType::GreaterEqual:
      case TokenType::EqualEqual:
      case TokenType::BangEqual:
        return 10;
      case TokenType::Plus:
      case TokenType::PlusEqual:
      case TokenType::Minus:
      case TokenType::MinusEqual:
        return 20;
      case TokenType::Star:
      case TokenType::StarEqual:
      case TokenType::Slash:
      case TokenType::SlashEqual:
        return 40;
      default:
        return -1;
    }
  };

  while (true) {
    int tokenPrec = getTokenPrecedence(peek().type);
    if (tokenPrec < exprPrec) {
      return lhs;
    }
    auto binOp = advance();
    auto rhs = TRY(parsePrimaryExpr());

    int nextPrec = getTokenPrecedence(peek().type);
    if (tokenPrec < nextPrec) {
      rhs = TRY(parseBinaryExprRHS(tokenPrec + 1, std::move(rhs)));

    }

    lhs = Expr::Binary(std::move(lhs), std::move(rhs), binOp);
  }
}

auto Parser::parseExpression() -> std::expected<Expr, ParseError> {
  auto lhs = TRY(parsePrimaryExpr());
  return parseBinaryExprRHS(0, std::move(lhs));
}

auto Parser::parsePrimaryExpr() -> std::expected<Expr, ParseError> {
  if (match({TokenType::Integer, TokenType::String, TokenType::True, TokenType::False})) {
    return Expr::Literal(peekPrev());
  }

  if (match(TokenType::Identifier)) {
    return Expr::Identifier(peekPrev());
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

auto Parser::advance() -> const Token& {
  mCurrent += 1;
  if (isAtEnd()) {
    return mTokens.back();
  }
  return mTokens.at(mCurrent-1);
}

auto Parser::peekPrev() const -> const Token& {
  if (mCurrent == 0) {
    RAISE_INTERNAL_ERROR("Tried to call `peekPrev` when mCurrent is 0");
  }
  return mTokens[mCurrent-1];
}

