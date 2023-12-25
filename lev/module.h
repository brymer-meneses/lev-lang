#pragma once

#include "diagnostics/sourceLocation.h"
#include <optional>
#include <string>
namespace lev {

struct Module {

  std::string filename;
  std::string source;

  constexpr Module(std::string filename, std::string source) 
    : filename(std::move(filename))
    , source(std::move(source)) {};

  Module(std::string_view filename);

  auto dumpSource() const -> void;

  auto getLine(size_t line) const -> std::optional<std::string>;

  auto getSourceFromLocation(SourceLocation location) const -> std::optional<std::string>;
};

}
