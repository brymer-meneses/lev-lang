#include "lev/compilation_unit.h"

#include <llvm/Support/raw_ostream.h>

#include "lev/lex/lex.h"

namespace Lev {

auto CompilationUnit::RunLexer() -> void {
  auto lex_result = Lex::Lex(source_, diagnostics_);

  token_buffer_ = std::move(lex_result.first);
  source_metadata_ = std::move(lex_result.second);

  if (diagnostics_.had_error()) {
    diagnostics_.ReportEverything(llvm::errs(), source_, *source_metadata_);
  }
}

auto CompilationUnit::DumpTokens() const -> void {
  for (const auto token : *token_buffer_) {
    auto position = source_metadata_->ConvertToLinePosition(
        token_buffer_->GetBufferPosition(token));

    auto kind = token_buffer_->GetKind(token);

    auto lexeme = token_buffer_->GetLexeme(token);

    llvm::outs() << "{" << " kind: " << '\'' << kind << '\''
                 << " lexeme: " << '\'' << lexeme << '\''
                 << ", line: " << position.line_number
                 << ", column_start: " << position.column_start
                 << ", column_end: " << position.column_end << " }" << "\n";
  }
}

}  // namespace Lev
