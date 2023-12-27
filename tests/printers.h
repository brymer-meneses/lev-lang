#pragma once

#include <iostream>
#include <lev/parsing/token.h>

namespace lev {

auto operator<<(std::ostream& stream, const lev::TokenType& tokenType) -> std::ostream&;

}
