cmake_minimum_required(VERSION 3.28)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Lev)

find_package(LLVM 18.1 REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(lev lev/driver.cpp)

target_compile_options(lev 
  PUBLIC 
  # -Wall 
  # -Wextra
  # -Wpedantic
  -fno-rtti
  -fno-exceptions
)

add_library(liblev
  STATIC
  lev/lex/lex.cpp
  lev/lex/token_buffer.cpp
  lev/lex/token_kind.cpp
  lev/parse/parse.cpp
  lev/compilation_unit.cpp
  lev/diagnostics_consumer.cpp
  lev/source/source_metadata.cpp
)

target_include_directories(liblev PUBLIC "." ${LLVM_INCLUDE_DIRS})

target_link_libraries(lev liblev ${llvm_libs})

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_runner lev/test_runner.cpp)

target_sources(
  test_runner 
  PRIVATE
  lev/source/source_metadata_test.cpp
  lev/lex/lex_test.cpp
)

target_link_libraries(
  test_runner
  liblev
  GTest::gtest_main
  ${llvm_libs}
)

include(GoogleTest)

