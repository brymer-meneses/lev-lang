#pragma once

#include <lev/sourceLocation.h>
#include <map>
#include <variant>

#include <lev/parsing/lexer.h>
#include <lev/diagnostics/errors.h>

namespace lev {

class Reporter {

  public:
    Reporter();
    auto report(LevError error) -> void;
};

}

