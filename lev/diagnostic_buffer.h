#ifndef LEV_DIAGNOSTIC_BUFFER_H
#define LEV_DIAGNOSTIC_BUFFER_H

#include <functional>
#include <vector>

#include "lev/diagnostic.h"
namespace Lev {

class DiagnosticBuffer {
 public:
  auto Add(Source::LinePosition,
           std::function<void(llvm::raw_ostream&)>) -> void;
  auto ReportEverything(llvm::raw_ostream& stream,
                        const Source& source) const -> void;

  auto had_error() const -> bool { return had_error_; }

 private:
  bool had_error_ = false;
  std::vector<Diagnostic> diagnostics_;
};

}  // namespace Lev

#endif  // !LEV_DIAGNOSTIC_BUFFER_H
