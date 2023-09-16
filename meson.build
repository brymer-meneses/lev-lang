project('lev', 'cpp')

sources = [
  'src/scanner.cc'
]

cpp_args = [
  '-std=c++2b',
  '-Wpedantic',
  '-fno-exceptions',
]

liblev = shared_library(
  'liblev',
  sources: sources,
  cpp_args: cpp_args,
)

executable(
  'lev', 
  sources: 'src/main.cc', 
  cpp_args: cpp_args, 
  link_with: liblev
)

gtest = subproject('gtest')
gtest_dep = gtest.get_variable('gtest_dep')
gmock_dep = gtest.get_variable('gmock_dep')

test_sources = [
  'tests/test_runner.cc',
  'tests/test_scanner.cc'
]

test_runner = executable(
  'lev-tester',
  sources: test_sources,
  dependencies: [
    gtest_dep,
    gmock_dep
  ],
  cpp_args: cpp_args,
  link_with: liblev,
  include_directories: ['src'],
)

test('lev-tester', test_runner)
