#ifndef LEV_SOURCE_METADATA_H
#define LEV_SOURCE_METADATA_H

#include <llvm/ADT/SmallVector.h>

#include "lev/common/types.h"

namespace Lev {

struct BufferPosition;
struct LinePosition;

class SourceMetadata {
 public:
  struct LineInfo {
    u32 start;
    u32 end;

    LineInfo(u32 start, u32 end) : start(start), end(end) {}
  };

  auto RegisterLineInfo(u32 start, u32 end) -> void;

  auto ConvertToLinePosition(BufferPosition position) const -> LinePosition;

  /// Gets the column number of an index to the source file
  auto GetLineColumnOffset(const u32 buffer_offset) const -> u32;

  /// Gets the line number of an index to the source file
  auto GetLineNumber(const u32 buffer_offset) const -> u32;

  auto GetLineInfo(u32 line_number) const -> LineInfo;

 private:
  std::vector<LineInfo> line_infos_;
};

struct LinePosition {
  u32 column_start;
  u32 column_end;
  u32 line_number;

  LinePosition(u32 column_start, u32 column_end, u32 line_number)
      : column_start(column_start),
        column_end(column_end),
        line_number(line_number) {}
};

struct BufferPosition {
  u32 start;
  u32 end;

  BufferPosition(u32 buffer_start, u32 buffer_end)
      : start(buffer_start), end(buffer_end) {}
};

}  // namespace Lev

#endif  // !LEV_SOURCE_METADATA_H
