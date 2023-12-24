#pragma once

#include <lev/parsing/token.h>
#include <lev/parsing/ast.h>
#include <lev/diagnostics/sourceLocation.h>

static SourceLocation TEST_LOCATION("testing.lev", 0, 0, 1);

auto verifyTokens(std::string_view source,
                  const std::initializer_list<lev::TokenType> types) -> void;

auto verifyTokens(std::string_view source,
                  const std::initializer_list<const char*> lexemes,
                  const std::initializer_list<lev::TokenType> types) -> void;

auto verifyStatement(std::string_view source, const lev::Stmt& expectedStatement) -> void;

namespace lev {

auto operator<<(std::ostream& stream, const lev::TokenType& token) -> std::ostream&;

}
