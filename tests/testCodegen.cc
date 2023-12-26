#include <gtest/gtest.h>

#include "testHelpers.h"

TEST(Codegen, FunctionDeclaration) {

  auto source = 
R"(
fn main() -> i32:
  let variable: i32 = 5
  return variable
)";

  verifyResult(source, 5);
}
