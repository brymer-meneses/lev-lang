#include <lev/diagnostic.h>

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
                              Source::LinePosition position) -> void {
  auto line_width = GetNumberWidth(position.line);

  stream << std::string(line_width + 3 + position.column.start, ' ') << '^'
         << "\n";
}

auto Diagnostic::Report(llvm::raw_ostream& stream, const Source& source,
                        const Source::LineOffsets& offsets) const -> void {
  const auto line_offsets = offsets.GetSourceLine(position_.line);
  const auto line =
      source.contents().slice(line_offsets.first + 1, line_offsets.second);

  stream << " " << position_.line << " | " << line << "\n";
  HighlightPosition(stream, position_);

  stream << "ERROR: ";
  writer_(stream);

  stream << "\n\n";
}

}  // namespace Lev
