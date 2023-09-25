project('lev', 'cpp', version: '0.0.1')

sources = [
  'src/ast.cc',
  'src/scanner.cc',
  'src/parser.cc',
  'src/codegen.cc',
  'src/context.cc'
]

cpp_args = [
  '-std=c++2b',
  '-Wpedantic',
  '-fno-exceptions',
  '-fno-rtti',
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
)

executable(
  'lev', 
  sources: 'src/main.cc', 
  cpp_args: cpp_args, 
  link_with: liblev,
  dependencies: [
    llvm_dep,
  ],
)

gtest = subproject('gtest')
gtest_dep = gtest.get_variable('gtest_dep')
gmock_dep = gtest.get_variable('gmock_dep')

test_sources = [
  'tests/test_runner.cc',
  'tests/test_scanner.cc',
  'tests/test_parser.cc',
  'tests/test_ast.cc',
  'tests/test_codegen.cc',
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
  include_directories: ['src'],
)

test('lev-tester', test_runner)
