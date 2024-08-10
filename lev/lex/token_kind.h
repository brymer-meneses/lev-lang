#ifndef LEV_TOKEN_KIND_H
#define LEV_TOKEN_KIND_H

#include <lev/common.h>
#include <llvm/Support/raw_ostream.h>

enum class TokenKind : u16 {

#define LEV_SYMBOL_TOKEN(kind, _) kind,

#define LEV_META_TOKEN(kind, _) LEV_SYMBOL_TOKEN(kind, _)
#define LEV_KEYWORD_TOKEN(kind, _) LEV_SYMBOL_TOKEN(kind, _)
#define LEV_LITERAL_TOKEN(kind, _) LEV_SYMBOL_TOKEN(kind, _)

#include "lev/lex/token_kind.def"

};

auto operator<<(llvm::raw_ostream& OS,
                const TokenKind obj) -> llvm::raw_ostream&;

#endif  // !LEV_TOKEN_KIND_H
