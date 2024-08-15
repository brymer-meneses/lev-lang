#include "lev/lex/lex.h"

#include <gtest/gtest.h>
#include <llvm/Support/MemoryBuffer.h>

#include <cstring>

#include "lev/diagnostics_consumer.h"
#include "lev/source/source.h"
#include "lev/source/source_metadata.h"

TEST(Lex, SourceMetadata) {
  llvm::StringRef data =
      "//0123456789\n"
      "//0123456789\n";

  auto memory_buffer = llvm::MemoryBuffer::getMemBuffer(data);
  auto source = Lev::Source{"", std::move(memory_buffer)};
  auto diagnostics = Lev::DiagnosticsConsumer{};

  auto lex_result = Lev::Lex::Lex(source, diagnostics);
  auto source_metadata = lex_result.second;

  auto line_infos = source_metadata.line_infos();

  // the first line is from [0, 13) we include the '\n' character here
  EXPECT_EQ(line_infos[0], Lev::BufferPosition(0, 13));

  // the second line is from [13, 25) we include the '\n' character here
  EXPECT_EQ(line_infos[1], Lev::BufferPosition(13, 26));
}
