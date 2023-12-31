project('lev', 'cpp', version: '0.0.1')

sources = [
  'lev/parsing/lexer.cc',
  'lev/parsing/ast.cc',
  'lev/parsing/parser.cc',

  'lev/diagnostics/reporter.cc',
  'lev/diagnostics/errors.cc',
  'lev/diagnostics/sourceLocation.cc',

  'lev/codegen/compiler.cc',
  'lev/codegen/context.cc',
  'lev/codegen/scope.cc',

  'lev/module.cc',
  'lev/executable.cc',
  'lev/lev.cc',
]

cpp_args = [
  '-std=c++2b',
  '-Wpedantic',
  '-fno-exceptions',
  '-fno-rtti',
  '-Wno-gnu-statement-expression'
]

llvm_dep = dependency(
  'llvm',
  static: false,
  required: true,
  modules: ['core', 'support', 'native', 'irreader'],
  version: '>=17.0.1',
)

if not llvm_dep.found()
  error('LLVM not found')
endif

liblev = static_library(
  'lev',
  sources: sources,
  cpp_args: cpp_args,
  dependencies: [
    llvm_dep,
  ],
  include_directories: ['lev']
)

executable(
  'lev', 
  sources: 'lev/main.cc', 
  cpp_args: cpp_args, 
  link_with: liblev,
)

gtest = subproject('gtest')
gtest_dep = gtest.get_variable('gtest_dep')
gmock_dep = gtest.get_variable('gmock_dep')

test_sources = [
  'tests/testRunner.cc',
  'tests/testLexer.cc',
  'tests/testParser.cc',
  'tests/testCodegen.cc',

  'tests/testHelpers.cc',
]

test_runner = executable(
  'lev-tester',
  sources: test_sources,
  dependencies: [
    gtest_dep,
    gmock_dep,
    llvm_dep,
  ],
  cpp_args: cpp_args,
  link_with: liblev,
  include_directories: ['lev'],
)

test('lev-tester', test_runner)
