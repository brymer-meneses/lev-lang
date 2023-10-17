#pragma once
#include <variant>
#include <format>

#include <lev/sourceLocation.h>
#include <lev/parsing/token.h>

namespace lev {

class LexingError {
  public:
    struct UnexpectedCharacter {
      char character;
      SourceLocation location;

      UnexpectedCharacter(char character, SourceLocation location)
        : character(character)
        , location(location) {}
    };

    struct RedundantDecimalPoint {
      SourceLocation location;
      RedundantDecimalPoint(SourceLocation location)
        : location(location) {}
    };

    struct UnterminatedString {
      SourceLocation location;
      UnterminatedString(SourceLocation location)
        : location(location) {}
    };

  private:
    using Error = std::variant<UnexpectedCharacter, RedundantDecimalPoint, UnterminatedString>;
    Error mError;

  public:
    template <class T>
    requires std::is_constructible_v<Error, T>
    LexingError(T error) : mError(error) {}

    constexpr auto location() const -> SourceLocation {
      return std::visit([](const auto& e) { return e.location; }, mError);
    }

    auto accept(auto visitor) -> decltype(auto) {
      return std::visit(visitor, mError);
    }

    auto message() const -> std::string;
};

class ParsingError {
  public:
    struct UnexpectedToken {
      TokenType got;
      SourceLocation location;

      UnexpectedToken(TokenType got, SourceLocation location)
        : got(got)
        , location(location) {}

      auto format() const -> std::string {
        return std::format("Unexpected token");
      }
    };

  private:
    using Error = std::variant<UnexpectedToken>;
    Error mError;

  public:
    template <class T>
    requires std::is_constructible_v<Error, T>
    ParsingError(T error) : mError(error) {}

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


struct LevError {
  using Error = std::variant<LexingError>;

  Error error;
  std::string_view line;

  template <class T>
  requires std::is_constructible_v<Error, T>
  LevError(T error, std::string_view line) 
    : error(error)
    , line(line) {}

  constexpr auto location() -> SourceLocation {
    return std::visit([](const auto& error){ return error.location(); }, error);
  }
  constexpr auto message() -> std::string {
    return std::visit([](const auto& error){ return error.message(); }, error);
  }
};

}