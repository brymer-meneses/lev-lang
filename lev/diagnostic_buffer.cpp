#include <lev/diagnostic_buffer.h>

#include <functional>

#include "lev/source.h"

namespace Lev {

auto DiagnosticBuffer::Add(Source::LinePosition position,
                           std::function<void(llvm::raw_ostream&)> writer)
    -> void {
  had_error_ = true;
  diagnostics_.emplace_back(position, writer);
}

auto DiagnosticBuffer::ReportEverything(llvm::raw_ostream& stream,
                                        const Source& source) const -> void {
  auto offsets = source.line_offsets();

  for (const auto& diagnostic : diagnostics_) {
    diagnostic.Report(stream, source, offsets);
  }
}

}  // namespace Lev
