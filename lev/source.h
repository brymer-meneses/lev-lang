#ifndef LEV_SOURCE_H
#define LEV_SOURCE_H

#include <lev/common.h>
#include <llvm/Support/MemoryBuffer.h>

#include <memory>
#include <string>
#include <utility>

class Source {
 public:
  class LineOffsets;
  struct LinePosition;

  auto filename() const -> llvm::StringRef { return filename_; }
  auto contents() const -> llvm::StringRef { return contents_->getBuffer(); }

  auto line_offsets() const -> Source::LineOffsets;

  explicit Source(std::string filename,
                  std::unique_ptr<llvm::MemoryBuffer> contents)
      : filename_(filename), contents_(std::move(contents)) {}

 private:
  std::string filename_;
  std::unique_ptr<llvm::MemoryBuffer> contents_;
};

class Source::LineOffsets {
 public:
  LineOffsets(const Source& source);

  auto GetLine(const u32 x) const -> std::optional<u32>;
  auto GetColumn(const u32 x) const -> std::optional<u32>;

  auto GetLineOffset(const u32 line) const -> std::optional<u32>;

  /// Gets the begining and end offsets from the source buffer of the entire
  /// line. These begin and end offsets can them be used to get the source of
  /// the entire line
  auto GetSourceLine(u32 line_number) const -> std::pair<u32, u32>;

  std::vector<u32> line_offsets_;
};

struct Source::LinePosition {
  struct Column {
    u32 start;
    u32 end;

    Column(u32 start, u32 end) : start(start), end(end){};
  };

  Column column;
  u32 line;

  LinePosition(u32 col_start, u32 col_end, u32 line)
      : column(col_start, col_end), line(line) {}
};

#endif  // !LEV_SOURCE_H
