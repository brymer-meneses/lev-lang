#include "lev/lex/token_buffer.h"

#include "lev/lex/token_kind.h"

namespace Lev::Lex {

auto TokenBuffer::GetLexeme(TokenId token) const -> llvm::StringRef {
  assert(token < starts_.size());

  switch (GetKind(token)) {
#define LEV_LITERAL_TOKEN(kind, string) \
  case TokenKind::kind:                 \
    return source_->contents().slice(starts_[token], ends_[token]);
#define LEV_SYMBOL_TOKEN(kind, string) \
  case TokenKind::kind:                \
    return string;
#define LEV_META_TOKEN(kind, string) \
  case TokenKind::kind:              \
    return "";
#define LEV_KEYWORD_TOKEN(kind, string) LEV_SYMBOL_TOKEN(kind, string)
#include "lev/lex/token_kind.def"
  }
}

auto TokenBuffer::AppendToken(TokenKind kind, u32 start, u32 end) -> void {
  kinds_.push_back(kind);
  starts_.push_back(start);
  ends_.push_back(end);
}

auto TokenBuffer::GetKind(TokenId token) const -> TokenKind {
  assert(token < kinds_.size());

  return kinds_[token];
}

auto TokenBuffer::GetLinePosition(const SourceMetadata& source_metadata,
                                  TokenId token) const -> LinePosition {
  assert(token < starts_.size());

  auto column_start = source_metadata.GetLineColumnOffset(starts_[token]);
  auto column_end = source_metadata.GetLineColumnOffset(ends_[token]);
  auto line = source_metadata.GetLineNumber(starts_[token]);

  return LinePosition(column_start, column_end, line);
}

}  // namespace Lev::Lex
