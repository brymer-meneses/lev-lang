#ifndef LEV_DIAGNOSTIC_H
#define LEV_DIAGNOSTIC_H

#include <lev/source/source.h>
#include <lev/source/source_metadata.h>
#include <llvm/Support/raw_ostream.h>

#include <functional>

namespace Lev {

class Diagnostic {
 public:
  Diagnostic(LinePosition position,
             std::function<void(llvm::raw_ostream&)> writer)
      : position_(position), writer_(writer) {}

  auto Report(llvm::raw_ostream& ostream, const Source& source,
              const SourceMetadata& offsets) const -> void;

 private:
  LinePosition position_;
  std::function<void(llvm::raw_ostream&)> writer_;
};

}  // namespace Lev

#endif  // !LEV_DIAGNOSTIC_H
