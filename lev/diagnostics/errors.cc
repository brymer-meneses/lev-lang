#include "errors.h"
#include "utils.h"

using namespace lev;

auto LexError::message() const -> std::string {
  return std::visit(utils::match {
    [](const RedundantDecimalPoint& e) {
      return std::format("Got redundant decimal point");
    },
    [](const UnterminatedString& e) {
      return std::format("Got an unterminated string");
    },
    [](const UnexpectedCharacter& e) {
      return std::format("Unexpected character {}", e.character);
    },
  }, mError);
}
