#ifndef LEV_TOKEN_KIND_H
#define LEV_TOKEN_KIND_H

#include <llvm/Support/raw_ostream.h>

#include "lev/common/types.h"

namespace Lev::Lex {

enum class TokenKind : u16 {

#define LEV_SYMBOL_TOKEN(kind, ...) kind,

#include "lev/lex/token_kind.def"

};

}  // namespace Lev::Lex

auto operator<<(llvm::raw_ostream& OS,
                const Lev::Lex::TokenKind obj) -> llvm::raw_ostream&;

#endif  // !LEV_TOKEN_KIND_H
