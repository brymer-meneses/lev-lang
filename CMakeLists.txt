cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Lev)

find_package(LLVM 18.1 REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(lev lev/driver.cpp)
target_sources(lev 
  PUBLIC
  lev/lex/lex.cpp
  lev/lex/token.cpp
  lev/compilation_unit.cpp
  lev/diagnostic.cpp
  lev/diagnostic_buffer.cpp
  lev/source.cpp
)

target_include_directories(lev PUBLIC ${LLVM_INCLUDE_DIRS})
target_include_directories(lev PUBLIC ".")
# target_compile_options(lev PUBLIC -Wall -Wextra -Wpedantic)

target_link_libraries(lev ${llvm_libs})
