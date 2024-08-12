#ifndef LEV_TOKEN_BUFFER_H
#define LEV_TOKEN_BUFFER_H

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

#include <vector>

#include "lev/common/types.h"
#include "lev/lex/token_kind.h"
#include "lev/source/source.h"
#include "lev/source/source_metadata.h"

namespace Lev {

using TokenId = u32;

class TokenIterator
    : public llvm::iterator_facade_base<
          TokenIterator, std::random_access_iterator_tag, const TokenId, int> {
  explicit TokenIterator(TokenId token) : token_(token) {}

 public:
  TokenIterator() = delete;

  auto operator==(const TokenIterator& rhs) const -> bool {
    return token_ == rhs.token_;
  }
  auto operator<=>(const TokenIterator& rhs) const -> std::strong_ordering {
    return token_ <=> rhs.token_;
  }

  auto operator*() const -> const TokenId& { return token_; }

  using iterator_facade_base::operator-;
  auto operator-(const TokenIterator& rhs) const -> int {
    return token_ - rhs.token_;
  }

  auto operator+=(int n) -> TokenIterator& {
    token_ += n;
    return *this;
  }
  auto operator-=(int n) -> TokenIterator& {
    token_ -= n;
    return *this;
  }

 private:
  friend class TokenBuffer;

  TokenId token_;
};

class TokenBuffer {
 public:
  explicit TokenBuffer(const Source& source) : source_(&source) {}

  auto AppendToken(TokenKind kind, u32 start, u32 end) -> void;

  auto GetKind(TokenId token) const -> TokenKind;
  auto GetLexeme(TokenId token) const -> llvm::StringRef;

  auto GetLinePosition(const SourceMetadata&,
                       TokenId token) const -> LinePosition;

  auto begin() const -> TokenIterator { return TokenIterator(0); }
  auto end() const -> TokenIterator { return TokenIterator(starts_.size()); }

  auto size() const -> u64 { return starts_.size(); }

 private:
  const Source* source_;

  std::vector<u32> starts_;
  std::vector<u32> ends_;
  std::vector<TokenKind> kinds_;
};

}  // namespace Lev

#endif  // !LEV_TOKEN_BUFFER_H
