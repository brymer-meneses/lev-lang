#ifndef LEV_DIAGNOSTIC_CONSUMER_H
#define LEV_DIAGNOSTIC_CONSUMER_H

#include <functional>
#include <vector>

#include "lev/source/source.h"
#include "lev/source/source_metadata.h"

namespace Lev {

class Diagnostic {
 public:
  Diagnostic(BufferPosition position,
             std::function<void(llvm::raw_ostream&)> writer)
      : position_(position), writer_(writer) {}

  auto Report(llvm::raw_ostream& ostream, const Source& source,
              const SourceMetadata& offsets) const -> void;

 private:
  BufferPosition position_;
  std::function<void(llvm::raw_ostream&)> writer_;
};

class DiagnosticsConsumer {
 public:
  auto Add(BufferPosition, std::function<void(llvm::raw_ostream&)>) -> void;

  auto ReportEverything(llvm::raw_ostream& stream, const Source& source,
                        const SourceMetadata& offsets) const -> void;

  auto had_error() const -> bool { return had_error_; }

 private:
  bool had_error_ = false;
  std::vector<Diagnostic> diagnostics_;
};

}  // namespace Lev

#endif  // !LEV_DIAGNOSTIC_BUFFER_H
