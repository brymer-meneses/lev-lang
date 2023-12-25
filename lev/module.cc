#include <lev/module.h>

#include <sstream>
#include <fstream>

#include <print>

using namespace lev;

Module::Module(std::string_view filename){
  std::ifstream file(filename);
  std::stringstream source;

  if (file.fail()) {
    std::println(stderr, "File `{}` does not exist", filename);
    exit(1);
  }

  if (file.is_open()) {
    std::string line;
    while (file) {
      std::getline(file, line);
      source << line << "\n";
    }
  }

  this->source = source.str();
};

auto Module::dumpSource() const -> void {
  std::println(stderr, "{}", source);
}

auto Module::getSourceFromLocation(SourceLocation location) const -> std::optional<std::string> {
  auto line = getLine(location.line);
  if (not line.has_value()) {
    return std::nullopt;
  }
  return line->substr(location.start, location.end - location.start);
}

auto Module::getLine(size_t line) const -> std::optional<std::string> {

  std::istringstream iss(source);
  std::string lineSource;

  for (auto i = 1; i <= line; i++) {
    if (not std::getline(iss, lineSource)) {
      return std::nullopt;
    }
  }

  return lineSource;
}
