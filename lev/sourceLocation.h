#pragma once

#include <cstddef>
#include <string_view>

struct SourceLocation {
  std::string_view filename = "anonymous";
  size_t start = 0;
  size_t end = 0;
  size_t line = 0;

  constexpr SourceLocation(std::string_view filename, size_t start, size_t end, size_t line)
    : filename(filename) 
    , start(start)
    , end(end)
    , line(line) {}
};

auto operator+(const SourceLocation& l1, const SourceLocation& l2) -> SourceLocation;
