#include <algorithm>
#include <print>

#include <lev/utils.h>
#include <lev/sourceLocation.h>

auto operator+(const SourceLocation& l1, const SourceLocation& l2) -> SourceLocation {
  if (l1.filename != l2.filename) {
    RAISE_INTERNAL_ERROR("cannot two source locations with different filenames left: {} right: {}", l1.filename, l2.filename);
  }

  if (l1.line != l2.line) {
    RAISE_INTERNAL_ERROR("cannot add two source locations with different lines left: {} right: {}", l1.line, l2.line);
  }

  return SourceLocation(l1.filename, std::min(l1.start, l2.start), std::max(l1.end, l2.end), l1.line);
}
