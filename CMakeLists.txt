cmake_minimum_required (VERSION 3.10)

project (
  OPTIONPP
  VERSION 2.0
  DESCRIPTION "C++ library for reading command-line options"
  )

set (OPTIONPP_VERSION "${PROJECT_VERSION}")

add_executable (
  run_tests
  src/option.cpp
  src/parser.cpp
  test/tst_main.cpp
  test/tst_option.cpp
  test/tst_parser.cpp
  )
target_include_directories (
  run_tests
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
  )
add_test (NAME run_tests COMMAND run_tests)

# Require standard C++11
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

# Set max warning level
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(run_tests PRIVATE -Wall -Wextra -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(run_tests PRIVATE /W4)
endif ()
