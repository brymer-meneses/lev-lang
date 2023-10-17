#pragma once
#include <variant>
#include <concepts>
#include "token.h"

namespace lev {

class BuiltinType {
  public:
    enum class Types {
      i8,
      u8,
      i16,
      u16,
      i32,
      u32,
      i64,
      u64,
      f32,
      f64,
    };

    Types type;

  public:
    static constexpr auto i8() -> BuiltinType {
      return BuiltinType(Types::i8);
    }
    static constexpr auto i16() -> BuiltinType {
      return BuiltinType(Types::i16);
    }
    static constexpr auto i32() -> BuiltinType {
      return BuiltinType(Types::i32);
    }
    static constexpr auto i64() -> BuiltinType {
      return BuiltinType(Types::i64);
    }

    static constexpr auto u8() -> BuiltinType {
      return BuiltinType(Types::u8);
    }
    static constexpr auto u16() -> BuiltinType {
      return BuiltinType(Types::u16);
    }
    static constexpr auto u32() -> BuiltinType {
      return BuiltinType(Types::u32);
    }
    static constexpr auto u64() -> BuiltinType {
      return BuiltinType(Types::u64);
    }

    static constexpr auto f32() -> BuiltinType {
      return BuiltinType(Types::f32);
    }
    static constexpr auto f64() -> BuiltinType {
      return BuiltinType(Types::f64);
    }

  private:
    constexpr BuiltinType(Types type) : type(type) {};
};

class GenericType {

};

class Inferred {};

class UserDefinedType {
  Token identifier;
};

class LevType {
  using Type = std::variant<BuiltinType, GenericType, UserDefinedType, Inferred>;

  public:

    template <typename T>
    requires std::is_constructible_v<Type, T>
    LevType(T type) : type(type) {};

    template <typename T>
    requires std::is_constructible_v<Type, T>
    auto is() const -> bool {
      return std::holds_alternative<T>(type);
    }

  private:
    Type type;
};

};
