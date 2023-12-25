#pragma once
#include <expected>
#include <vector>

#include <lev/parsing/token.h>
#include <lev/parsing/lexer.h>
#include <lev/diagnostics/reporter.h>
#include <lev/diagnostics/sourceLocation.h>
#include <lev/executable.h>

namespace lev {

class Lev {

public:
  Lev(const char** argv);
  auto compile() -> void;

  auto lex() -> std::vector<Token>;
  auto reset() -> void;

private:
  Lexer mLexer;
  Reporter mReporter;
  Executable mExecutable;
  
private:
  auto processCommandLineArgs(const char**) -> void;

};

}

