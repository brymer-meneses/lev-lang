#pragma once
#include <expected>
#include <vector>
#include "parsing/token.h"
#include "parsing/lexer.h"

#include "reporter.h"
#include "sourceContext.h"


class Lev {

public:
  Lev(const char** argv);
  auto compile() -> void;

  auto lex() -> std::vector<Token>;
  auto reset() -> void;

private:
  Lexer mLexer;
  Reporter mReporter;
  SourceContext mSourceContext;
  
private:
  auto processCommandLineArgs(const char**) -> void;

};
