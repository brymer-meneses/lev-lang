#ifndef LEV_LEXER_H
#define LEV_LEXER_H

#include <llvm/ADT/StringRef.h>

#include <expected>

#include "lev/diagnostics_consumer.h"
#include "lev/lex/token_buffer.h"
#include "lev/source/source_metadata.h"

namespace Lev::Lex {

auto Lex(const Source&,
         DiagnosticsConsumer&) -> std::pair<Lex::TokenBuffer, SourceMetadata>;

}  // namespace Lev::Lex

#endif  // !LEV_LEXER_H
