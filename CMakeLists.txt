cmake_minimum_required (VERSION 3.10)

project (
  OPTIONPP
  VERSION 2.0
  DESCRIPTION "C++ library for reading command-line options"
  )

set (OPTIONPP_NAME "Option++")
set (OPTIONPP_VERSION "${PROJECT_VERSION}")
set (OPTIONPP_DESCRIPTION "${PROJECT_DESCRIPTION}")

option (OPTIONPP_DOCS "Generate documentation" ON)

# Require standard C++11
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

# See if Doxygen is available
if (OPTIONPP_DOCS)
  find_package (Doxygen)
  if (DOXYGEN_FOUND)
    # Generate Doxygen config file
    set (DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
    set (DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

    configure_file ("${DOXYGEN_IN}" "${DOXYGEN_OUT}")

    add_custom_target (doc ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      VERBATIM
      )
  else () # No Doxygen
    message ("Doxygen not found, documentation will not be generated")
  endif ()
endif ()

add_executable (
  run_tests
  src/option.cpp
  src/parser.cpp
  src/parser_result.cpp
  src/utility.cpp
  test/tst_main.cpp
  test/tst_option.cpp
  test/tst_parser.cpp
  test/tst_parser_result.cpp
  test/tst_utility.cpp
  )
target_include_directories (
  run_tests
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
  )
add_test (NAME run_tests COMMAND run_tests)

# Set max warning level
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(run_tests PRIVATE -Wall -Wextra -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(run_tests PRIVATE /W4)
endif ()
