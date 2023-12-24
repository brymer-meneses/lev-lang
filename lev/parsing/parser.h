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
    size_t mCurrent = 0;

  public:
    Parser() = default;
    auto setTokens(std::vector<Token>) -> void;
    auto parse() -> std::expected<std::vector<Stmt>, ParseError>;

  private:
    auto parseStatement() -> std::expected<Stmt, ParseError>;
    auto parseBlockStmt() -> std::expected<Stmt, ParseError>;
    auto parseReturnStmt() -> std::expected<Stmt, ParseError>;
    auto parseControlStmt() -> std::expected<Stmt, ParseError>;
    auto parseAssignmentStmt() -> std::expected<Stmt, ParseError>;

    auto parseDeclaration() -> std::expected<Stmt, ParseError>;
    auto parseVariableDeclaration() -> std::expected<Stmt, ParseError>;
    auto parseFunctionDeclaration() -> std::expected<Stmt, ParseError>;

    auto parseExpression() -> std::expected<Expr, ParseError>;
    auto parseBinaryExprRHS(int exprPrec, Expr lhs) -> std::expected<Expr, ParseError>;
    auto parseLiteralExpr() -> std::expected<Expr, ParseError>;
    auto parseUnaryExpr() -> std::expected<Expr, ParseError>;

    auto parseFunctionArgument() -> std::expected<FunctionArgument, ParseError>;
    auto parseType() -> std::expected<LevType, ParseError>;

    auto match(TokenType type) -> bool;
    auto match(std::initializer_list<TokenType> types) -> bool;
    auto check(TokenType type) const -> bool;

    auto expect(TokenType type, std::string_view message) -> std::expected<Token, ParseError>;
    auto expect(TokenType type) -> std::expected<Token, ParseError>;

    auto getCurrentLocation() -> SourceLocation;
    auto advance() -> const Token&;
    auto peek() const -> const Token&;
    auto peekPrev() const -> const Token&;
    auto isAtEnd() const -> bool;
};

}
