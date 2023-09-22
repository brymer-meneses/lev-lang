#include "codegen.h"
#include "parser.h"

#include <string>
#include <gtest/gtest.h>
#include "token.h"

using namespace lev::codegen;
using namespace lev::parser;

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

TEST(Codegen, FunctionDeclaration) {
  Codegen codegen;
  codegen.compile(R"(
fn main(a: i32) -> i32:
    let num: i32 = 5
)"
  );

  std::cerr << codegen.dump() << std::endl;

  std::string_view result =
R"(; ModuleID = 'lev'
source_filename = "lev"

define i32 @main() {
entry:
  %num = alloca i32
  store i32 5, i32* %num
  ret i32 0
}
)";

  EXPECT_EQ(codegen.dump(), result);
}