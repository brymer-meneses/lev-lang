#include "codegen.h"

#include <string>
#include <gtest/gtest.h>
#include "token.h"

using namespace lev::codegen;

TEST(Codegen, Module) {

  Codegen codegen;

  FunctionDeclaration func("main", {}, {}, lev::ast::Type::i8);

  codegen.visit(func);

  std::string_view result = 
R"(; ModuleID = 'lev'
source_filename = "lev"

define i8 @main() {
entry:
  ret i8 0
}
)";

  EXPECT_EQ(codegen.dump(), 
              result
            );
}

