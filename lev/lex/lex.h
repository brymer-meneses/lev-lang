#ifndef LEV_LEXER_H
#define LEV_LEXER_H

#include <lev/diagnostic.h>
#include <lev/diagnostic_buffer.h>
#include <lev/lex/token_buffer.h>
#include <lev/source/source_metadata.h>
#include <llvm/ADT/StringRef.h>

#include <expected>

namespace Lev {

auto Lex(const Source&,
         DiagnosticBuffer&) -> std::pair<TokenBuffer, SourceMetadata>;

}  // namespace Lev

#endif  // !LEV_LEXER_H
