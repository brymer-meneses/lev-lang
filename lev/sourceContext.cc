
#include <print>
#include <fstream>

#include <lev/utils.h>
#include <lev/sourceContext.h>

using namespace lev;

auto SourceContext::getLine(std::string_view filename, size_t line) const -> std::optional<std::string> {
  auto key = std::string(filename);

  if (not mFilenamesToSourceLines.contains(key)) {
    return std::nullopt;
  }

  return mFilenamesToSourceLines.at(key).at(line-1);
}


auto SourceContext::dumpAllSources() const -> void {
  for (const auto& [filename, source] : mFilenamesToSourceLines) {
    std::println("filename: {}", filename);

    for (const auto& line : mFilenamesToSourceLines.at(filename)) {
      std::println("{}", line);
    }
  }
}

auto SourceContext::getSourceLineFromLocation(SourceLocation location) const -> std::optional<std::string_view> {
  auto key = std::string(location.filename);

  if (not mFilenamesToSourceLines.contains(key)) {
    return std::nullopt;
  }

  return mFilenamesToSourceLines.at(key).at(location.line-1).substr(location.start, location.end - location.start);
}

auto SourceContext::addSourceFile(std::string_view filename) -> void {
  std::ifstream file(filename);

  if (mFilenamesToSourceLines.contains(std::string(filename))) {
    RAISE_INTERNAL_ERROR("The file {} already exists in the source context.", filename);
  }

  if (!file.is_open()) {
    RAISE_INTERNAL_ERROR("Invalid file {}", filename);
  }

  std::string line;
  while (std::getline(file, line)) {
    mFilenamesToSourceLines[std::string(filename)].push_back(std::move(line));
  }
}

auto SourceContext::reset() -> void {
  mFilenamesToSourceLines.clear();
}
