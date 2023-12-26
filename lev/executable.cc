
#include <print>
#include <ranges>

#include <fstream>

#include <lev/misc/macros.h>
#include <lev/executable.h>

using namespace lev;

auto Executable::getLine(std::string_view filename, size_t line) const -> std::optional<std::string> {
  auto module = findModuleFromFilename(filename);
  if (not module.has_value()) {
    return std::nullopt;
  }
  return module->getLine(line);
}

auto Executable::dumpAllSources() const -> void {
  for (const auto& module : modules) {
    module.dumpSource();
  }
}

auto Executable::getSourceLineFromLocation(SourceLocation location) const -> std::optional<std::string> {
  auto module = findModuleFromFilename(location.filename);
  if (not module.has_value()) {
    return std::nullopt;
  }
  return module->getSourceFromLocation(location);
}

auto Executable::addModule(Module module) -> void {
  modules.push_back(module);
}

auto Executable::findModuleFromFilename(std::string_view filename) const -> std::optional<Module> {
  auto condition = [&filename](const Module& m) {
    return m.filename == filename;
  };

  auto it = std::ranges::find_if(modules, condition);
  if (it == modules.end()) {
    return std::nullopt;
  }
  return *it;
}
