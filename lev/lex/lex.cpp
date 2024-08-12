
#include "lev/lex/lex.h"

#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>

#include <utility>

#include "lev/diagnostic_buffer.h"
#include "lev/lex/token_buffer.h"
#include "lev/source/source.h"
#include "lev/source/source_metadata.h"

namespace Lev::Lex {

class [[clang::internal_linkage]] Lexer {
 public:
  Lexer(const Source& source, DiagnosticBuffer& diagnostics)
      : diagnostics_(diagnostics),
        token_buffer_(source),
        source_(source.contents()) {}

  auto Lex() -> std::pair<TokenBuffer, SourceMetadata> {
    CreateToken(TokenKind::FileStart);

    while (!IsAtEnd()) {
      LexToken();

      start_ = current_;
    }

    CreateToken(TokenKind::FileEnd);

    return std::make_pair(token_buffer_, source_metadata_);
  }

  auto LexToken() -> void {
    auto c = Advance();

    switch (c) {
      case '\n': {
        line_ += 1;
        column_ = 0;

        source_metadata_.RegisterLineInfo(line_start_, current_ - 1);

        line_start_ = current_;
        break;
      }

      case ' ':
      case '\t':
      case '\r': {
        break;
      }

      case '"': {
        LexString();
        break;
      }

      case ',': {
        CreateToken(TokenKind::Comma);
        break;
      }

      case ':': {
        CreateToken(TokenKind::Colon);
        break;
      }

      case '(': {
        CreateToken(TokenKind::LeftParen);
        break;
      }

      case ')': {
        CreateToken(TokenKind::RightParen);
        break;
      }

      case '[': {
        CreateToken(TokenKind::LeftBracket);
        break;
      }

      case ']': {
        CreateToken(TokenKind::RightBracket);
        break;
      }

      case '{': {
        CreateToken(TokenKind::LeftBrace);
        break;
      }

      case '}': {
        CreateToken(TokenKind::RightBrace);
        break;
      }

      case ';': {
        CreateToken(TokenKind::Semicolon);
        break;
      }

      case '+': {
        if (Match('=')) {
          CreateToken(TokenKind::PlusEqual);
        } else {
          CreateToken(TokenKind::Plus);
        }
        break;
      }

      case '-': {
        if (Match('=')) {
          CreateToken(TokenKind::MinusEqual);
        } else {
          CreateToken(TokenKind::Minus);
        }
        break;
      }

      case '/': {
        if (Match('/')) {
          while (Peek() != '\n') {
            Advance();
          }

        } else if (Match('=')) {
          CreateToken(TokenKind::SlashEqual);
        } else {
          CreateToken(TokenKind::Slash);
        }
        break;
      }

      case '*': {
        if (Match('=')) {
          CreateToken(TokenKind::StarEqual);
        } else {
          CreateToken(TokenKind::Star);
        }
        break;
      }

      case '>': {
        if (Match('=')) {
          CreateToken(TokenKind::GreaterEqual);
        } else {
          CreateToken(TokenKind::Greater);
        }
        break;
      }

      case '<': {
        if (Match('=')) {
          CreateToken(TokenKind::LesserEqual);
        } else {
          CreateToken(TokenKind::Lesser);
        }
        break;
      }

      case '=': {
        if (Match('=')) {
          CreateToken(TokenKind::EqualEqual);
        } else {
          CreateToken(TokenKind::Equal);
        }
        break;
      }

      default: {
        if (llvm::isAlpha(c) or c == '_') {
          LexKeyword();

        } else if (llvm::isDigit(c) or c == '.') {
          auto next = Peek();

          if (llvm::isDigit(next) or next == '.') {
            LexNumber();
          } else {
            CreateToken(TokenKind::Dot);
          }

        } else {
          auto position = LinePosition(/*column_start*/ column_,
                                       /*column_end*/ column_, /*line*/ line_);

          diagnostics_.Add(position, [=](llvm::raw_ostream& os) {
            os << "Invalid character: " << "\"" << c << "\"";
          });
        }
      }
    }
  }

  auto LexKeyword() -> void {
    while (llvm::isAlnum(Peek())) {
      Advance();
    }

    auto kind = ClassifyKeyword(source_.slice(start_, current_));
    CreateToken(kind);
  }

  auto LexString() -> void {
    while (not IsAtEnd() and Peek() != '"') {
      Advance();
    }

    if (Peek() != '"' and IsAtEnd()) {
      auto position = LinePosition(/*column_start*/ column_,
                                   /*column_end*/ column_, /*line*/ line_);

      diagnostics_.Add(position, [=](llvm::raw_ostream& os) {
        os << "Unterminated string";
      });
      return;
    }

    // Consume the last "
    Advance();
    CreateToken(TokenKind::StringLiteral);
  }

  auto LexNumber() -> void {
    auto did_visit_dot = false;

    auto IsPossibleNumber = [](const char c) {
      return llvm::isDigit(c) or c == '.';
    };

    while (not IsAtEnd() and IsPossibleNumber(Peek())) {
      auto c = Advance();

      if (did_visit_dot and c == '.') {
        auto position = LinePosition(/*column_start*/ column_,
                                     /*column_end*/ column_, /*line*/ line_);

        diagnostics_.Add(position, [=](llvm::raw_ostream& os) {
          os << "Unexpected symbol: `.`, a number can't have more than one "
                "decimal point.";
        });
        return;
      }

      if (c == '.') {
        did_visit_dot = true;
      }
    }

    CreateToken(did_visit_dot ? TokenKind::RealLiteral
                              : TokenKind::IntegerLiteral);
  }

  auto ClassifyKeyword(llvm::StringRef source) -> TokenKind {
#define LEV_SYMBOL_TOKEN(...)
#define LEV_KEYWORD_TOKEN(kind, str) \
  if (source == str) {               \
    return TokenKind::kind;          \
  }
#include <lev/lex/token_kind.def>
    return TokenKind::Identifier;
  }

  auto Advance() -> char {
    if (IsAtEnd()) {
      return '\0';
    }

    current_ += 1;
    column_ += 1;
    return source_[current_ - 1];
  }

  auto Peek() -> char {
    if (IsAtEnd()) {
      return '\0';
    }

    return source_[current_];
  }

  auto Match(const char expected) -> bool {
    if (Peek() == expected) {
      Advance();
      return true;
    }

    return false;
  }

  auto IsAtEnd() -> bool { return current_ >= source_.size(); }

  auto CreateToken(TokenKind kind) -> void {
    token_buffer_.AppendToken(kind, start_, current_);
  }

 private:
  u32 line_ = 0;
  u32 start_ = 0;
  u32 column_ = 0;
  u32 current_ = 0;

  u32 line_start_ = 0;

  DiagnosticBuffer& diagnostics_;
  TokenBuffer token_buffer_;
  SourceMetadata source_metadata_;

  llvm::StringRef source_;

};  // namespace Lev

auto Lex(const Source& source,
         DiagnosticBuffer& buffer) -> std::pair<TokenBuffer, SourceMetadata> {
  auto lexer = Lexer(source, buffer);
  return lexer.Lex();
}

}  // namespace Lev::Lex
