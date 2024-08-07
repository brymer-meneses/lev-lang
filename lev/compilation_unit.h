
#ifndef LEV_COMPILATION_UNIT_H
#define LEV_COMPILATION_UNIT_H

#include <lev/diagnostic_buffer.h>
#include <lev/lex/token.h>
#include <lev/source.h>
#include <llvm/Support/MemoryBuffer.h>

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
  std::optional<std::vector<Token>> tokens_;
  DiagnosticBuffer diagnostics_;
  Source source_;
};

}  // namespace Lev

#endif  // !LEV_COMPILATION_UNIT_H
