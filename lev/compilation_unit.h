
#ifndef LEV_COMPILATION_UNIT_H
#define LEV_COMPILATION_UNIT_H

#include <llvm/Support/MemoryBuffer.h>

#include "lev/diagnostics_consumer.h"
#include "lev/lex/token_buffer.h"
#include "lev/source/source.h"
#include "lev/source/source_metadata.h"

namespace Lev {

class CompilationUnit {
 public:
  explicit CompilationUnit(Source source) : source_(std::move(source)) {}

  auto RunLexer() -> void;
  auto RunParser() -> void;

  auto DumpTokens() const -> void;

  auto filename() const -> llvm::StringRef { return source_.filename(); }
  auto source() const -> llvm::StringRef { return source_.contents(); }

 private:
  std::optional<Lex::TokenBuffer> token_buffer_;
  std::optional<SourceMetadata> source_metadata_;

  DiagnosticsConsumer diagnostics_;
  Source source_;
};

}  // namespace Lev

#endif  // !LEV_COMPILATION_UNIT_H
