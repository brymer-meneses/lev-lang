#include "codegen.h"

#include <string>
#include <gtest/gtest.h>
#include "token.h"

using namespace lev::codegen;

TEST(Codegen, GlobalVariable) {
  Codegen codegen;
  codegen.compile("let global_variable: i32 = 5");

  std::string_view result = 
R"(; ModuleID = 'lev'
source_filename = "lev"

@global_variable = common global i32, align 4
)";

  EXPECT_EQ(codegen.dump(), result);
}
