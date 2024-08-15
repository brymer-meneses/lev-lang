#include "lev/diagnostics_consumer.h"

#include <llvm/Support/raw_ostream.h>

#include <functional>

#include "lev/source/source.h"
#include "lev/source/source_metadata.h"

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

  stream << std::string(line_width + 4 + position.column_start, ' ') << '^'
         << "\n";
}

auto DiagnosticsConsumer::ReportEverything(
    llvm::raw_ostream& stream, const Source& source,
    const SourceMetadata& offsets) const -> void {
  for (const auto& diagnostic : diagnostics_) {
    diagnostic.Report(stream, source, offsets);
  }
}

auto DiagnosticsConsumer::Add(BufferPosition position,
                              std::function<void(llvm::raw_ostream&)> func)
    -> void {
  had_error_ = true;
  diagnostics_.emplace_back(position, func);
}

auto Diagnostic::Report(llvm::raw_ostream& stream, const Source& source,
                        const SourceMetadata& source_metadata) const -> void {
  const auto line_pos = source_metadata.ConvertToLinePosition(position_);
  const auto line_offsets =
      source_metadata.GetBufferPositionOfLineNumber(line_pos.line_number);
  const auto line =
      source.contents().slice(line_offsets.start, line_offsets.end - 1);

  stream << " " << line_pos.line_number + 1 << " | " << line << "\n";
  HighlightPosition(stream, line_pos);

  stream << "ERROR: ";
  writer_(stream);

  stream << "\n\n";
}

}  // namespace Lev
