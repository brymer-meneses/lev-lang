
#include "lev/lex/token_kind.h"

auto operator<<(llvm::raw_ostream& os,
                Lev::Lex::TokenKind kind) -> llvm::raw_ostream& {
  switch (kind) {
#define LEV_SYMBOL_TOKEN(kind, string) \
  case Lev::Lex::TokenKind::kind:      \
    os << #kind;                       \
    break;
#include "lev/lex/token_kind.def"
  }

  return os;
}
