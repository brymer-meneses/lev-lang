#pragma once
#include <vector>

#include "token.h"
#include <string_view>
#include <string>
#include <variant>
#include <expected>
#include <concepts>
#include <format>

#include "../utils.h"

class LexingError {
  public:
    struct UnexpectedCharacter {
      char character;
      SourceLocation location;

      UnexpectedCharacter(char character, SourceLocation location)
        : character(character)
        , location(location) {}

      auto format() const -> std::string {
        return std::format("Unexpected character {}", character);
      }
    };

    struct RedundantDecimalPoint {
      SourceLocation location;
      RedundantDecimalPoint(SourceLocation location)
        : location(location) {}

      auto format() const -> std::string {
        return std::format("Got redundant decimal point");
      }
    };

    struct UnterminatedString {
      SourceLocation location;
      UnterminatedString(SourceLocation location)
        : location(location) {}

      auto format() const -> std::string {
        return std::format("Got an unterminated string");
      }
    };

  private:
    using Error = std::variant<UnexpectedCharacter, RedundantDecimalPoint, UnterminatedString>;
    Error mError;

  public:
    template <class T>
    requires std::is_constructible_v<Error, T>
    LexingError(T error) : mError(error) {}

    auto accept(auto visitor) -> decltype(auto) {
      return std::visit(visitor, mError);
    }

    auto location() const -> SourceLocation {
      return std::visit([](const auto& e) { return e.location; }, mError);
    }

    auto message() const -> std::string {
      return std::visit([](const auto& e){ return e.format(); }, mError);
    }
};

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

    auto getCurrentLocation() -> SourceLocation;
    auto getCharLocation(int offset=0) -> SourceLocation;

    auto advance() -> char;
    [[nodiscard]] auto buildToken(TokenType type) -> Token;
    [[nodiscard]] auto isAtEnd() const -> bool;
    [[nodiscard]] auto match(const char expected) -> bool;
    [[nodiscard]] auto peek() const -> char;
    [[nodiscard]] auto peekPrev() const -> char;

};
