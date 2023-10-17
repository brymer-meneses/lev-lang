#include "reporter.h"
#include <iostream>
#include <iomanip>
#include <print>

#include "lev.h"

using namespace lev;

Reporter::Reporter() {}

auto Reporter::report(LevError error) -> void {
  const auto location = error.location();
  const auto indent = std::to_string(location.line).length();
  const auto source = error.line;

  auto padding = std::string("");

  padding.insert(0, indent, ' ');

  std::println("error {}: {}", location.filename, error.message());
  std::println("{} |", padding);
  std::println("{} | {}", location.line, source);
  std::println("{} |", padding);
}
