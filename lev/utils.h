#pragma once
#include <string_view>
#include <source_location>
#include <print>

namespace lev::utils {
  template<class... Ts>
  struct match : Ts... { using Ts::operator()...; };
  // explicit deduction guide (not needed as of C++20)
  template<class... Ts>
  match(Ts...) -> match<Ts...>;
}

#define RAISE_INTERNAL_ERROR(...) \
{ \
  const auto location = std::source_location::current(); \
  std::println(stderr, "[{}:{}]: {}", location.file_name(), location.line(), location.function_name()); \
  std::println(stderr, "INTERNAL ERROR: " __VA_ARGS__); \
  exit(1); \
} \
