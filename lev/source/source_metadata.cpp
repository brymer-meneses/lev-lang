#include "lev/source/source_metadata.h"

#include <llvm/ADT/STLExtras.h>

#include <algorithm>
#include <iterator>

namespace Lev {

auto SourceMetadata::RegisterLineInfo(const u32 start,
                                      const u32 length) -> void {
  line_infos_.emplace_back(start, length);
}

auto SourceMetadata::GetLineColumnOffset(const u32 index) const -> u32 {
  if (line_infos_.empty() or index < line_infos_.front().start) {
    return index;
  }

  auto it =
      std::ranges::find_if(line_infos_, [index](LineInfo line_info) -> bool {
        return line_info.start <= index and index < line_info.end;
      });

  if (it == line_infos_.end()) {
    return 0;
  }

  return index - it->start;
}

auto SourceMetadata::GetLineNumber(const u32 index) const -> u32 {
  auto it =
      std::ranges::find_if(line_infos_, [index](LineInfo line_info) -> bool {
        return line_info.start <= index and index < line_info.end;
      });

  return std::distance(line_infos_.begin(), it);
}

auto SourceMetadata::GetLineInfo(const u32 line) const -> LineInfo {
  if (line > line_infos_.size() or line_infos_.empty()) {
    return LineInfo(0, 0);
  }

  return line_infos_[line];
}

auto SourceMetadata::ConvertToLinePosition(BufferPosition buffer_position) const
    -> LinePosition {
  auto line_number = GetLineNumber(buffer_position.start);
  auto column_start = GetLineColumnOffset(buffer_position.start);
  auto column_end = GetLineColumnOffset(buffer_position.end);

  return LinePosition(column_start, column_end, line_number);
}

}  // namespace Lev
