#pragma once

#include <vector>
#include <expected>
#include <optional>
#include <memory>

#include "token.h"
#include "ast.h"

namespace lev::parser {

  using namespace lev::ast;
  using namespace lev::token;

  struct UnexpectedToken {
    TokenType required;
    TokenType found;

    UnexpectedToken(TokenType required, TokenType found)
        : required(required), found(found) {}
  };

  using ParserError = std::variant<UnexpectedToken>;

  class Parser {
    private:
      std::vector<Token> mTokens;
      size_t mCurrent = 0;
    
    public:
      Parser(std::vector<Token> tokens);
      Parser(std::string_view source);

      auto parse() -> std::expected<std::vector<Stmt>, ParserError>;
      static auto printError(ParserError error) -> void;

    private:
      auto peek() const -> const Token&;
      auto advance() -> const Token&;
      auto peekPrev() const -> const Token&;
      auto isAtEnd() const -> bool;

      auto buildError() -> void;

      auto match(TokenType type) -> bool;
      auto expect(TokenType type) -> std::optional<Token>;

      auto parseDeclaration() -> std::expected<Stmt, ParserError>;
      auto parseFunctionDeclaration() -> std::expected<Stmt, ParserError>;
      auto parseVariableDeclaration() -> std::expected<Stmt, ParserError>;
      
      auto parseBinaryOpRHS(int opPrecedence, Expr lhs) -> std::expected<Expr, ParserError>;

      auto parseBlock() -> std::expected<Stmt, ParserError>;
      auto parseStmt() -> std::expected<Stmt, ParserError>;
      auto parseType() -> std::expected<Type, ParserError>;

      auto parseExpr() -> std::expected<Expr, ParserError>;
      auto parsePrimaryExpr() -> std::expected<Expr, ParserError>;
  };

}
