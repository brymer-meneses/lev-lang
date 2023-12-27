
#include <lev/parsing/lexer.h>
#include <lev/parsing/parser.h>

#include <tests/printers.h>

#include <llvm/ExecutionEngine/Interpreter.h>

#include <gtest/gtest.h>
#include <ranges>

using namespace lev;

auto verifyTokens(std::string_view source,
                  const std::initializer_list<TokenType> types) -> void {

  Lexer lexer;
  lexer.setSource(source);

  auto tokens = lexer.lex();
  if (not tokens) {
    FAIL() << tokens.error().message();
  }

  for (const auto& [token, type] : std::views::zip(*tokens, types)) {
    EXPECT_EQ(token.type, type);
  }
}

auto verifyTokens(std::string_view source,
                  const std::initializer_list<const char*> lexemes,
                  const std::initializer_list<TokenType> types) -> void {

  ASSERT_EQ(lexemes.size(), types.size()) << "expected lexemes and types must have the same size";

  Lexer lexer;
  lexer.setSource(source);

  auto tokens = lexer.lex();
  if (not tokens) {
    FAIL() << tokens.error().message();
  }

  ASSERT_EQ(tokens->size(), lexemes.size() + 1);

  for (const auto& [token, lexeme, type] : std::views::zip(*tokens, lexemes, types)) {
    EXPECT_EQ(token.lexeme, lexeme) << std::format("Got mismatched lexeme expected: `{}` got: `{}`", lexeme, token.lexeme);
  }
}

auto verifyStatement(std::string_view source, const Stmt& expectedStatement) -> void {
  Lexer lexer;
  lexer.setSource(source);
  auto tokens = lexer.lex();

  ASSERT_TRUE(tokens) << tokens.error().message();

  Parser parser;
  parser.setTokens(std::move(*tokens));
  auto statements = parser.parse();

  ASSERT_TRUE(statements) << statements.error().message();
  ASSERT_EQ(statements->size(), 1);
  
  EXPECT_EQ(*statements->at(0), expectedStatement);
}

// auto verifyResult(std::string_view source, int result) -> void {
//   Lexer lexer(source, "test.lev");
//   auto tokens = lexer.lex();
//
//   ASSERT_TRUE(tokens.has_value()) << tokens.error().message();
//
//   Parser parser;
//   parser.setTokens(std::move(*tokens));
//
//   auto statements = parser.parse();
//
//   ASSERT_TRUE(statements) << statements.error().message();
//   ASSERT_EQ(statements->size(), 1);
//
//   Compiler compiler(std::move(*statements));
//   auto status = compiler.compile();
//   if (not status) {
//     FAIL() << status.error().message();
//   }
//
//   auto ir = compiler.dump();
//   auto module = compiler.getModule();
//   llvm::Function* func = module->getFunction("main");
//
//   std::unique_ptr<llvm::ExecutionEngine> executionEngine (
//     llvm::EngineBuilder(std::move(module)).create()
//   );
//
//   int gotResult = executionEngine->runFunctionAsMain(func, {}, nullptr);
//
//   EXPECT_EQ(result, gotResult) << ir;
// }
