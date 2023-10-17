#include <fstream>
#include <sstream>
#include <print>
#include <assert.h>

#include <lev/lev.h>

using namespace lev;

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

    if (not sourceLine) {
      RAISE_INTERNAL_ERROR("Error getting source line from location");
    }

    mReporter.report({error, *sourceLine});
  }
  return *tokens;
}
