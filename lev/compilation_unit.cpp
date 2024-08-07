#include <lev/compilation_unit.h>
#include <lev/lex/lex.h>
#include <lev/source.h>
#include <llvm/Support/raw_ostream.h>

namespace Lev {

auto CompilationUnit::RunLexer() -> void {
  tokens_ = Lex(source_.contents(), diagnostics_);

  if (diagnostics_.had_error()) {
    diagnostics_.ReportEverything(llvm::errs(), source_);
  }
}

auto CompilationUnit::DumpTokens() const -> void {
  auto offsets = source_.line_offsets();

  for (const auto token : *tokens_) {
    auto line = offsets.GetLine(token.start);
    auto col_start = offsets.GetColumn(token.start);
    auto col_end = offsets.GetColumn(token.end);

    auto lexeme = token.lexeme(source_.contents());

    llvm::outs() << "{" << " lexeme: " << '\"' << lexeme << "\""
                 << ", line: " << *line + 1 << ", column_start: " << *col_start
                 << ", column_end: " << *col_end << " }" << "\n";
  }
}

}  // namespace Lev
