#pragma once

#include <vector>
#include <expected>
#include <format>

#include <lev/parsing/token.h>
#include <lev/parsing/ast.h>
#include <lev/diagnostics/errors.h>

namespace lev {

class Parser {

  private:
    std::vector<Token> mTokens;
    size_t mCurrent;

  public:
    Parser() = default;
    auto setTokens(std::vector<Token>) -> void;
    auto parse() -> std::expected<std::vector<Stmt>, ParsingError>;

  private:
    auto parseStatement() -> std::expected<Stmt, ParsingError>;
    auto parseBlockStmt() -> std::expected<Stmt, ParsingError>;
    auto parseDeclaration() -> std::expected<Stmt, ParsingError>;

    auto parseVariableDeclaration() -> std::expected<Stmt, ParsingError>;
    auto parseFunctionDeclaration() -> std::expected<Stmt, ParsingError>;

    auto parseExpression() -> std::expected<Expr, ParsingError>;
    auto parseBinaryExpr() -> std::expected<Expr, ParsingError>;
    auto parseLiteralExpr() -> std::expected<Expr, ParsingError>;
    auto parseUnaryExpr() -> std::expected<Expr, ParsingError>;

    auto match(TokenType type) -> bool;
    auto match(std::initializer_list<TokenType> types) -> bool;
    auto getCurrentLocation() -> SourceLocation;
    auto peek() const -> const Token&;
    auto peekPrev() const -> const Token&;
    auto isAtEnd() const -> bool;
};

}
