#pragma once
#include <vector>

#include <string_view>
#include <string>
#include <variant>
#include <expected>
#include <concepts>
#include <format>

#include <lev/utils.h>
#include <lev/diagnostics/errors.h>
#include <lev/parsing/token.h>

namespace lev {

class Lexer {

  private:
    std::string_view mSource;
    std::string_view mFilename = "anonymous";
    size_t mCurrent = 0;
    size_t mStart = 0;
    size_t mLine = 1;
    size_t mLastNewline = 0;

  public:
    Lexer() = default;

    auto setSource(std::string_view) -> void;
    auto setFilename(std::string_view) -> void;

    auto lex() -> std::expected<std::vector<Token>, LexingError>;
    auto reset() -> void;

  private:
    auto lexNextToken() -> std::expected<Token, LexingError>;
    auto lexNumber() -> std::expected<Token, LexingError>;
    auto lexString() -> std::expected<Token, LexingError>;
    auto lexIdentifier() -> std::expected<Token, LexingError>;
    auto lexIndentation() -> std::expected<Token, LexingError>;

    auto skipWhitespaces() -> void;
    auto skipComments() -> void;

    auto getCurrentLocation() -> SourceLocation;
    auto getPrevCharLocation() -> SourceLocation;
    auto getCurrentCharLocation() -> SourceLocation;

    auto advance() -> char;
    [[nodiscard]] auto buildToken(TokenType type) -> Token;
    [[nodiscard]] auto isAtEnd() const -> bool;

    [[nodiscard]] auto match(const char expected) -> bool;
    [[nodiscard]] auto match(std::initializer_list<const char>) -> bool;

    [[nodiscard]] auto check(const char expected) const -> bool;
    [[nodiscard]] auto check(std::initializer_list<const char>) const -> bool;

    [[nodiscard]] auto peek() const -> char;
    [[nodiscard]] auto peekNext() const -> char;
    [[nodiscard]] auto peekPrev() const -> char;

};

}
