#ifndef LEV_TOKEN_H
#define LEV_TOKEN_H

#include <lev/common.h>
#include <lev/source.h>
#include <llvm/ADT/StringRef.h>

namespace Lev {

enum class TokenKind : u16 {

#define LEV_SYMBOL_TOKEN(kind, _) kind,

#define LEV_META_TOKEN(kind, _) LEV_SYMBOL_TOKEN(kind, _)
#define LEV_KEYWORD_TOKEN(kind, _) LEV_SYMBOL_TOKEN(kind, _)
#define LEV_LITERAL_TOKEN(kind, _) LEV_SYMBOL_TOKEN(kind, _)

#include "lev/lex/token_kind.def"

};

auto operator<<(llvm::raw_ostream& OS,
                const TokenKind obj) -> llvm::raw_ostream&;

struct Token {
  explicit Token(TokenKind kind, u32 start, u32 end)
      : kind(kind), start(start), end(end){};

  auto lexeme(llvm::StringRef source) const -> llvm::StringRef;
  auto line_position(const Source::LineOffsets& offsets) const
      -> Source::LinePosition;

  u32 start;
  u32 end;

  TokenKind kind;
};

}  // namespace Lev

#endif
