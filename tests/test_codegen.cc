#include "codegen.h"

#include <string>
#include <gtest/gtest.h>

using namespace lev::codegen;

TEST(CODEGEN, Module) {
  std::string Str;
  raw_string_ostream OS(Str);
  OS << *TheModule;
  OS.flush();
  
  EXPECT_EQ(Str, "; ModuleID = 'lev'\n"
                "source_filename = \"lev\"\n");
}

