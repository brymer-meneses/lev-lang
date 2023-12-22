#pragma once

#include <lev/diagnostics/errors.h>

namespace lev {

class Reporter {

  public:
    Reporter();
    auto report(LevError error) -> void;
};

}

