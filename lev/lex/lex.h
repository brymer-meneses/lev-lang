#ifndef LEV_LEXER_H
#define LEV_LEXER_H

#include <lev/diagnostic.h>
#include <lev/diagnostic_buffer.h>
#include <lev/lex/token.h>
#include <llvm/ADT/StringRef.h>

#include <expected>

namespace Lev {

auto Lex(llvm::StringRef, DiagnosticBuffer&) -> std::vector<Token>;

}  // namespace Lev

#endif  // !LEV_LEXER_H
