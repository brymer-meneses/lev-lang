#include <gtest/gtest.h>

#include "testHelpers.h"

TEST(Codegen, FunctionDeclaration) {

  auto source = 
R"(
fn main() -> i32:
  let variable: i32 = 5 + 2 * 10
  return variable
)";

  verifyResult(source, 25);
}

// TEST(Codegen, VariableAssignment) {
//
//   auto source = 
// R"(
// fn main() -> i32:
//   let mut variable: i32 = 5 + 2 * 10
//   variable = 10
//   return variable
// )";
//
//   verifyResult(source, 10);
// }

TEST(Codegen, ControlStatement) {

  auto source = 
R"(
fn main() -> i32:
  let variable: i32 = 10
  if variable == 10:
    return 5
  else if variable == 11:
    return 6
  else:
    return 7
)";
  
  verifyResult(source, 5);
}
