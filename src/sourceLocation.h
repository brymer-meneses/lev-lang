#pragma once

#include <cstddef>
#include <string_view>

class SourceLocation {

  public:
    std::string_view filename = "Anonymous";
    size_t start = 0;
    size_t end = 0;
    size_t line = 0;

    SourceLocation(std::string_view filename, size_t start, size_t end, size_t line)
      : filename(filename) 
      , start(start)
      , end(end)
      , line(line) {}

    SourceLocation(size_t start, size_t end, size_t line)
      : start(start)
      , end(end)
      , line(line) {}
};
