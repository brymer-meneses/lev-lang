#include "lev/lex/token_buffer.h"

#include "lev/lex/token_kind.h"
#include "lev/source/source_metadata.h"

namespace Lev::Lex {

auto TokenBuffer::GetLexeme(TokenId token) const -> llvm::StringRef {
  assert(token.value() < positions_.size());

  auto position = GetBufferPosition(token);

  switch (GetKind(token)) {
#define LEV_LITERAL_TOKEN(kind, string) \
  case TokenKind::kind:                 \
    return source_->contents().slice(position.start, position.end);
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
  positions_.emplace_back(start, end);
}

auto TokenBuffer::GetKind(TokenId token) const -> TokenKind {
  assert(token.value() < kinds_.size());

  return kinds_[token.value()];
}

auto TokenBuffer::GetBufferPosition(TokenId token) const -> BufferPosition {
  assert(token.value() < positions_.size());
  return positions_[token.value()];
}

}  // namespace Lev::Lex
