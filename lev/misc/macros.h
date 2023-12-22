#pragma once

#include <print>
#include <source_location>

#define RAISE_INTERNAL_ERROR(...) \
{ \
  const auto location = std::source_location::current(); \
  std::println(stderr, "[{}:{}]: {}", location.file_name(), location.line(), location.function_name()); \
  std::println(stderr, "INTERNAL ERROR: " __VA_ARGS__); \
  exit(1); \
} \

#define TRY(expr)                                                              \
  ({                                                                           \
    auto temp = expr;                                                          \
    if (not temp.has_value()) {                                                \
      return std::unexpected(temp.error());                                    \
    };                                                                         \
    std::move(*temp);                                                          \
  });
