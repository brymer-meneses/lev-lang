#include <lev/diagnostic_buffer.h>
#include <lev/source/source.h>
#include <lev/source/source_metadata.h>

#include <functional>

namespace Lev {

auto DiagnosticBuffer::Add(LinePosition position,
                           std::function<void(llvm::raw_ostream&)> writer)
    -> void {
  had_error_ = true;
  diagnostics_.emplace_back(position, writer);
}

auto DiagnosticBuffer::ReportEverything(
    llvm::raw_ostream& stream, const Source& source,
    const SourceMetadata& offsets) const -> void {
  for (const auto& diagnostic : diagnostics_) {
    diagnostic.Report(stream, source, offsets);
  }
}

}  // namespace Lev
