#ifndef LEV_SOURCE_H
#define LEV_SOURCE_H

#include <memory>
#include <string>
#include <utility>

#include "llvm/Support/MemoryBuffer.h"

namespace Lev {

class Source {
 public:
  auto filename() const -> llvm::StringRef { return filename_; }
  auto contents() const -> llvm::StringRef { return contents_->getBuffer(); }

  explicit Source(std::string filename,
                  std::unique_ptr<llvm::MemoryBuffer> contents)
      : filename_(filename), contents_(std::move(contents)) {}

 private:
  std::string filename_;
  std::unique_ptr<llvm::MemoryBuffer> contents_;
};

}  // namespace Lev

#endif  // !LEV_SOURCE_H
