#include <lev/source.h>

#include <iterator>

auto Source::line_offsets() const -> LineOffsets { return LineOffsets(*this); }

using LineOffsets = Source::LineOffsets;

LineOffsets::LineOffsets(const Source& source) {
  for (auto [i, c] : llvm::enumerate(source.contents())) {
    if (c == '\n') {
      line_offsets_.push_back(i);
    }
  }
}

auto LineOffsets::GetSourceLine(const u32 line_number) const
    -> std::pair<u32, u32> {
  auto start = GetLineOffset(line_number);
  auto end = GetLineOffset(line_number + 1);

  if (!start or !end) {
    return std::make_pair(0, 0);
  }

  return std::make_pair(*start, *end);
}

auto LineOffsets::GetLineOffset(const u32 line_number) const
    -> std::optional<u32> {
  for (auto [i, line_offset] : llvm::enumerate(line_offsets_)) {
    if (i == line_number) {
      return line_offset;
    }
  }

  return std::nullopt;
}

auto LineOffsets::GetLine(const u32 x) const -> std::optional<u32> {
  if (line_offsets_.empty() || x < line_offsets_.front()) {
    return x;
  }

  auto it = std::ranges::lower_bound(line_offsets_, x);
  return std::distance(line_offsets_.begin(), it);
}

auto LineOffsets::GetColumn(const u32 x) const -> std::optional<u32> {
  if (line_offsets_.empty() || x < line_offsets_.front()) {
    return x;
  }

  auto it = std::ranges::lower_bound(line_offsets_, x);
  if (it == line_offsets_.end()) {
    return std::nullopt;
  }

  auto index = std::distance(line_offsets_.begin(), it);
  auto last = (index > 0) ? line_offsets_[index - 1] : 0;

  return x - last - 1;
}
