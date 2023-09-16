project('lev', 'cpp')

sources = [
  'src/main.cc',
  'src/scanner.cc'
]

cpp_args = [
  '-std=c++2b',
  '-Wpedantic',
]

executable('lev', 
            sources: sources, 
            cpp_args: cpp_args, 
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
  ]
)

test('lev-tester', test_runner)

