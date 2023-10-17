#pragma once
#include <vector>
#include <map>

#include <lev/sourceLocation.h>

namespace lev {

class SourceContext {

  public:
    auto getLine(std::string_view filename, size_t line) const -> std::optional<std::string>;
    auto dumpAllSources() const -> void;

    auto getSourceLineFromLocation(SourceLocation location) const -> std::optional<std::string_view>;
    auto addSourceFile(std::string_view source) -> void;

    auto reset() -> void;

  private:
    using SourceLines = std::vector<std::string>;

    std::map<std::string, SourceLines> mFilenamesToSourceLines;
};

}
