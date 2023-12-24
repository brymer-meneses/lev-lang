#pragma once
#include <variant>
#include <concepts>
#include "token.h"

namespace lev {


class LevType {
  public:
    struct Builtin {
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

      static constexpr auto i8() -> Builtin {
        return Builtin(Types::i8);
      }
      static constexpr auto i16() -> Builtin {
        return Builtin(Types::i16);
      }
      static constexpr auto i32() -> Builtin {
        return Builtin(Types::i32);
      }
      static constexpr auto i64() -> Builtin {
        return Builtin(Types::i64);
      }

      static constexpr auto u8() -> Builtin {
        return Builtin(Types::u8);
      }
      static constexpr auto u16() -> Builtin {
        return Builtin(Types::u16);
      }
      static constexpr auto u32() -> Builtin {
        return Builtin(Types::u32);
      }
      static constexpr auto u64() -> Builtin {
        return Builtin(Types::u64);
      }

      static constexpr auto f32() -> Builtin {
        return Builtin(Types::f32);
      }
      static constexpr auto f64() -> Builtin {
        return Builtin(Types::f64);
      }
    private:
        constexpr Builtin(Types type) : type(type) {};
    };

    class Generic {

    };

    class Inferred {};

    struct UserDefined {
      Token identifier;
      constexpr explicit UserDefined(Token identifier) 
        : identifier(identifier) {}
    };

  using ValueType = std::variant<Builtin, Generic, UserDefined, Inferred>;

  public:

    template <typename T>
    requires std::is_constructible_v<ValueType, T>
    LevType(T value) : value(value) {};

    template <typename T>
    requires std::is_constructible_v<ValueType, T>
    auto is() const -> bool {
      return std::holds_alternative<T>(value);
    }

  private:
    ValueType value;

  friend constexpr auto operator==(const LevType&, const LevType&) -> bool;
};

};
