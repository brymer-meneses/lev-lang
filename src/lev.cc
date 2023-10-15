#include "lev.h"
#include <fstream>
#include <sstream>
#include <print>
#include <assert.h>

Lev::Lev(const char** argv) {
  processCommandLineArgs(argv);
}

auto Lev::processCommandLineArgs(const char** argv) -> void {
  std::vector<std::string_view> arguments;
  auto i = 1;
  while (argv[i] != nullptr) {
    arguments.emplace_back(argv[i]);
    i += 1;
  }

  if (i == 1) {
    std::println(stderr, "Error: No argument was passed");
    exit(1);
  }

  for (const auto& arg : arguments) {
    if (arg.contains(".lev"))
      mSourceContext.addSourceFile(arg);
  }
}

auto Lev::reset() -> void {
  mLexer.reset();
  mSourceContext.reset();
}

auto Lev::compile() -> void {

}

auto Lev::lex() -> std::vector<Token> {
  auto tokens = mLexer.lex();
  if (not tokens) {
    auto error = tokens.error();
    auto sourceLine = mSourceContext.getSourceLineFromLocation(error.location());

    assert(sourceLine.has_value());

    mReporter.report({error, *sourceLine});
    exit(1);
  }
  return *tokens;
}
