
#include "lev/lex/token_kind.h"

auto operator<<(llvm::raw_ostream& os, TokenKind kind) -> llvm::raw_ostream& {
  switch (kind) {
#define LEV_SYMBOL_TOKEN(kind, string) \
  case TokenKind::kind:                \
    os << #kind;                       \
    break;
#include "lev/lex/token_kind.def"
  }

  return os;
}
