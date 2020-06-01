cmake_minimum_required (VERSION 3.10)

project (
  OPTIONPP
  VERSION 2.0
  DESCRIPTION "C++ library for reading command-line options"
  )

set (OPTIONPP_VERSION "${PROJECT_VERSION}")

include_directories("${PROJECT_BINARY_DIR}")

# Require standard C++11
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

# Set max warning level
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(option++ PRIVATE -Wall -Wextra -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(option++ PRIVATE /W4)
endif ()
