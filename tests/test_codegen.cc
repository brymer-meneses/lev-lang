#include "codegen.h"

#include <string>
#include <gtest/gtest.h>
#include "token.h"

using namespace lev::codegen;

TEST(Codegen, GlobalVariable) {
  Codegen codegen;
  codegen.compile("let num: i32 = 5");

  std::string_view result = 
R"(; ModuleID = 'lev'
source_filename = "lev"

@num = global i32 5
)";

  EXPECT_EQ(codegen.dump(), result);
}

TEST(Codegen, LocalVariables) {
  Codegen codegen;
  codegen.compile(
R"(
fn main() -> i32:
  let num: i32 = 5
)");

  std::string_view result =
R"(; ModuleID = 'lev'
source_filename = "lev"

define i32 @main() {
entry:
  %num = alloca i32, align 4
  store i32 5, ptr %num, align 4
}
)";

  EXPECT_EQ(codegen.dump(), result);
}
