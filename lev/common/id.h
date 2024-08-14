#ifndef LEV_COMMON_ID_H
#define LEV_COMMON_ID_H

#include <compare>

#include "lev/common/types.h"

namespace Lev {

class Id {
 private:
  i32 value_;

 public:
  constexpr explicit Id(i32 value) : value_(value) {}

  constexpr auto value() const -> i32 { return value_; }

  constexpr auto operator+=(Id id1) -> void { value_ += id1.value_; }
  constexpr auto operator-=(Id id1) -> void { value_ -= id1.value_; }
};

constexpr auto operator<=>(Id id1, Id id2) -> std::strong_ordering {
  return id1.value() <=> id2.value();
}

constexpr auto operator==(Id id1, Id id2) -> bool {
  return id1.value() == id2.value();
}

constexpr auto operator-(Id id1, Id id2) -> Id {
  return Id(id1.value() - id2.value());
}

}  // namespace Lev

#endif  // !LEV_COMMON_ID_H
