#include <fstream>
#include <sstream>
#include <print>
#include <assert.h>

#include <lev/lev.h>
#include <lev/misc/macros.h>
#include <lev/misc/match.h>

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
      mExecutable.addModule(Module(arg));
  }
}

auto Lev::compile() -> void {
}

