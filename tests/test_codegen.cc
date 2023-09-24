#include "codegen.h"

#include <string>
#include <gtest/gtest.h>
#include "token.h"

using namespace lev::codegen;

TEST(Codegen, GlobalVariable) {
  Codegen codegen("let num: i32 = 5");
  codegen.compile();

  std::string_view result = 
R"(; ModuleID = 'lev'
source_filename = "lev"

@num = common constant i32 5, align 4
)";

  EXPECT_EQ(codegen.dump(), result);
}

TEST(Codegen, LocalVariables) {
  Codegen codegen(
R"(
fn main() -> i32:
  let num: i32 = 5
)");

  codegen.compile();

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

TEST(Codegen, SimpleBinaryExpr) {
  Codegen codegen(
R"(
fn main() -> i32:
  let num: i32 = 5 + 6 * 2
)");
  codegen.compile();

  std::string_view result =
R"(; ModuleID = 'lev'
source_filename = "lev"

define i32 @main() {
entry:
  %num = alloca i32, align 4
  store i32 17, ptr %num, align 4
}
)";
  EXPECT_EQ(codegen.dump(), result);
}

TEST(Codegen, SimpleBooleanExpr) {
  Codegen codegen("let value: bool = true");
  codegen.compile();

  std::string_view result = 
R"(; ModuleID = 'lev'
source_filename = "lev"

@value = common constant i1 true, align 4
)";

  EXPECT_EQ(codegen.dump(), result);
}
