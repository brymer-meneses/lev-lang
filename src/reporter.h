#pragma once

#include "sourceLocation.h"
#include <map>
#include <variant>

#include "parsing/lexer.h"

struct LevError {
  using Error = std::variant<LexingError>;

  Error error;
  std::string_view line;

  template <class T>
  requires std::is_constructible_v<Error, T>
  LevError(T error, std::string_view line) 
    : error(error)
    , line(line) {}

  auto location() -> SourceLocation {
    return std::visit([](const auto& error){ return error.location(); }, error);
  }

  auto message() -> std::string {
    return std::visit([](const auto& error){ return error.message(); }, error);
  }
};

class Reporter {

  public:
    Reporter();
    auto report(LevError error) -> void;
};


