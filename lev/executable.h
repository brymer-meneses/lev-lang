#pragma once
#include <vector>
#include <map>

#include <lev/diagnostics/sourceLocation.h>
#include <lev/module.h>

namespace lev {

class Executable {

public:
  auto getLine(std::string_view filename, size_t line) const -> std::optional<std::string>;
  auto dumpAllSources() const -> void;

  auto getSourceLineFromLocation(SourceLocation location) const -> std::optional<std::string>;
  auto addModule(Module module) -> void;

private:
  auto findModuleFromFilename(std::string_view filename) const -> std::optional<Module>;
  std::vector<Module> modules;
};

}
