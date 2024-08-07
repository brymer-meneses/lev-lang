#include "token.h"

namespace Lev {

auto Token::lexeme(llvm::StringRef source) const -> llvm::StringRef {
  switch (kind) {
#define LEV_LITERAL_TOKEN(kind, string) \
  case TokenKind::kind:                 \
    return source.slice(start, end);
#define LEV_SYMBOL_TOKEN(kind, string) \
  case TokenKind::kind:                \
    return string;
#define LEV_META_TOKEN(kind, string) LEV_SYMBOL_TOKEN(kind, string)
#define LEV_KEYWORD_TOKEN(kind, string) LEV_SYMBOL_TOKEN(kind, string)
#include "lev/lex/token_kind.def"
  }
}

auto Token::line_position(const Source::LineOffsets& offsets) const
    -> Source::LinePosition {
  const auto line = offsets.GetLine(start);
  const auto column_start = offsets.GetColumn(start);
  const auto column_end = offsets.GetColumn(end);

  return {
      .column =
          {
              .start = *column_start,
              .end = *column_end,
          },
      .line = *line,
  };
}

}  // namespace Lev
