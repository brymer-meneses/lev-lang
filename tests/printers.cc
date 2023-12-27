#include "printers.h"

auto lev::operator<<(std::ostream& stream, const lev::TokenType& tokenType) -> std::ostream& {
  return stream << Token::typeToString(tokenType);
}
