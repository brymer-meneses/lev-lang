#ifndef LEV_SOURCE_METADATA_H
#define LEV_SOURCE_METADATA_H

#include <llvm/ADT/SmallVector.h>

#include "lev/common/types.h"

namespace Lev {

struct LinePosition;

/// Represents a slice of the buffer defined by the range [start, end).
/// The `start` index is inclusive, and the `end` index is exclusive,
/// meaning the slice covers the elements from `start` up to, but not including,
/// `end`.
struct BufferPosition {
  u32 start;
  u32 end;

  BufferPosition(u32 buffer_start, u32 buffer_end)
      : start(buffer_start), end(buffer_end) {}

  constexpr auto operator==(const BufferPosition&) const -> bool = default;
};

class SourceMetadata {
 public:
  auto RegisterLineInfo(BufferPosition position) -> void;

  auto ConvertToLinePosition(BufferPosition position) const -> LinePosition;

  /// Gets the column number of an index to the source file
  auto GetLineColumnOffset(const u32 buffer_offset) const -> u32;

  /// Gets the line number of an index to the source file
  auto GetLineNumber(const u32 buffer_offset) const -> u32;

  auto GetBufferPositionOfLineNumber(u32 line_number) const -> BufferPosition;

  auto line_infos() const -> const std::vector<BufferPosition>& {
    return line_infos_;
  }

 private:
  std::vector<BufferPosition> line_infos_;
};

struct LinePosition {
  u32 column_start;
  u32 column_end;
  u32 line_number;

  LinePosition(u32 column_start, u32 column_end, u32 line_number)
      : column_start(column_start),
        column_end(column_end),
        line_number(line_number) {}

  constexpr auto operator==(const LinePosition&) const -> bool = default;
};

}  // namespace Lev

#endif  // !LEV_SOURCE_METADATA_H
