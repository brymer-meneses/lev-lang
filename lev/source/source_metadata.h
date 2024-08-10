#ifndef LEV_SOURCE_METADATA_H
#define LEV_SOURCE_METADATA_H

#include <lev/common.h>
#include <llvm/ADT/SmallVector.h>

namespace Lev {

class SourceMetadata {
 public:
  struct LineInfo {
    u32 start;
    u32 end;

    LineInfo(u32 start, u32 end) : start(start), end(end) {}
  };

  auto RegisterLineInfo(u32 start, u32 end) -> void;

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

}  // namespace Lev

#endif  // !LEV_SOURCE_METADATA_H
