#pragma once

#include <vector>
#include <expected>
#include <optional>
#include <memory>

#include "token.h"
#include "ast.h"
#include "visitor.h"

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

      auto parse() -> std::expected<std::vector<std::unique_ptr<Stmt>>, ParserError>;

    private:
      auto peek() const -> std::optional<Token>;
      auto peekPrev() const -> std::optional<Token>;
      auto isAtEnd() const -> bool;

      auto match(TokenType type) -> bool;
      auto expect(TokenType type) -> std::optional<Token>;

      auto parseDeclaration() -> std::expected<std::unique_ptr<Stmt>, ParserError>;
      auto parseFunctionDeclaration() -> std::expected<std::unique_ptr<Stmt>, ParserError>;
      auto parseVariableDeclaration() -> std::expected<std::unique_ptr<Stmt>, ParserError>;

      auto parseStmt() -> std::expected<std::unique_ptr<Stmt>, ParserError>;

      auto parseExpr() -> std::expected<std::unique_ptr<Expr>, ParserError>;
      auto parsePrimaryExpr() -> std::expected<std::unique_ptr<Expr>, ParserError>;
  };

}
