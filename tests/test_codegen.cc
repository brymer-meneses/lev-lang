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
  return 0
)");

  codegen.compile();

  std::string_view result =
R"(; ModuleID = 'lev'
source_filename = "lev"

define i32 @main() {
entry:
  %num = alloca i32, align 4
  store i32 5, ptr %num, align 4
  ret i32 0
}
)";

  EXPECT_EQ(codegen.dump(), result);
}

TEST(Codegen, SimpleBinaryExpr) {
  Codegen codegen(
R"(
fn main() -> i32:
  let num: i32 = 5 + 6 * 2
  return num
)");
  codegen.compile();

  std::string_view result =
R"(; ModuleID = 'lev'
source_filename = "lev"

define i32 @main() {
entry:
  %num = alloca i32, align 4
  store i32 17, ptr %num, align 4
  %num1 = load i32, ptr %num, align 4
  ret i32 %num1
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

TEST(Codegen, VariableExpr) {
  Codegen codegen(
R"(
fn main() -> i32:
  let num: i32 = 10
  let double_num: i32 = 2 * num
  return 0
)");

  codegen.compile();

  std::string_view result = 
R"(; ModuleID = 'lev'
source_filename = "lev"

define i32 @main() {
entry:
  %num = alloca i32, align 4
  store i32 10, ptr %num, align 4
  %double_num = alloca i32, align 4
  %num1 = load i32, ptr %num, align 4
  %multmp = mul i32 2, %num1
  store i32 %multmp, ptr %double_num, align 4
  ret i32 0
}
)";

  EXPECT_EQ(codegen.dump(), result);
}

TEST(Codegen, IfStmt) {

  Codegen codegen(
R"(
fn main() -> i32:
  let mut num: i32 = 10
  if num == 1:
    num = 11
  else if num == 2:
    num = 12
  else if num == 3:
    num = 13
  else:
    num = 14
  return 0
)");

  codegen.compile();

  std::string_view result = 
R"(; ModuleID = 'lev'
source_filename = "lev"

define i32 @main() {
entry:
  %num = alloca i32, align 4
  store i32 10, ptr %num, align 4
  %num1 = load i32, ptr %num, align 4
  %eetmp = icmp eq i32 %num1, 1
  %ifcond = icmp eq i1 %eetmp, true
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  store i32 11, ptr %num, align 4
  br label %ifend

else:                                             ; preds = %entry
  %num2 = load i32, ptr %num, align 4
  %eetmp3 = icmp eq i32 %num2, 2
  %ifcond4 = icmp eq i1 %eetmp3, true
  br i1 %ifcond4, label %then5, label %else6

then5:                                            ; preds = %else
  store i32 12, ptr %num, align 4
  br label %ifend

else6:                                            ; preds = %else
  %num7 = load i32, ptr %num, align 4
  %eetmp8 = icmp eq i32 %num7, 3
  %ifcond9 = icmp eq i1 %eetmp8, true
  br i1 %ifcond9, label %then10, label %else11

then10:                                           ; preds = %else6
  store i32 13, ptr %num, align 4
  br label %ifend

else11:                                           ; preds = %else6
  store i32 14, ptr %num, align 4
  br label %ifend

ifend:                                            ; preds = %else11, %then10, %then5, %then
  ret i32 0
}
)";
  EXPECT_EQ(codegen.dump(), result);
}
