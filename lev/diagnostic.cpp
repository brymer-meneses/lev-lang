#include "lev/diagnostic.h"

namespace Lev {

static auto GetNumberWidth(u32 number) -> u8 {
  auto number_of_digits = 0;
  do {
    ++number_of_digits;
    number /= 10;
  } while (number);

  return number_of_digits;
}

static auto HighlightPosition(llvm::raw_ostream& stream,
                              LinePosition position) -> void {
  auto line_width = GetNumberWidth(position.line_number);

  stream << std::string(line_width + 3 + position.column_start, ' ') << '^'
         << "\n";
}

auto Diagnostic::Report(llvm::raw_ostream& stream, const Source& source,
                        const SourceMetadata& source_metadata) const -> void {
  const auto line_offsets = source_metadata.GetLineInfo(position_.line_number);
  const auto line =
      source.contents().slice(line_offsets.start, line_offsets.end);

  stream << " " << position_.line_number + 1 << " | " << line << "\n";
  HighlightPosition(stream, position_);

  stream << "ERROR: ";
  writer_(stream);

  stream << "\n\n";
}

}  // namespace Lev
