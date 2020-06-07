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

# Export symbols on Windows
set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# See if Doxygen is available
if (OPTIONPP_DOCS)
  find_package (Doxygen)
  if (DOXYGEN_FOUND)
    # Generate Doxygen config file
    set (DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in")
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

set (OPTIONPP_SOURCE_FILES
  src/error.cpp
  src/option.cpp
  src/option_group.cpp
  src/parser.cpp
  src/parser_result.cpp
  src/result_iterator.cpp
  src/utility.cpp
  )

set (OPTIONPP_TEST_FILES
  test/tst_main.cpp
  test/tst_option.cpp
  test/tst_parser.cpp
  test/tst_parser_result.cpp
  test/tst_result_iterator.cpp
  test/tst_utility.cpp
  )

set (OPTIONPP_EXAMPLES
  docs/examples/basic.cpp
  )

# Build library
add_library (optionpp SHARED "${OPTIONPP_SOURCE_FILES}")
target_include_directories (optionpp PRIVATE include)

# Build test executable
add_executable (test "${OPTIONPP_TEST_FILES}")
target_link_libraries (test PRIVATE optionpp)
target_include_directories (test PRIVATE include third_party)
add_test (NAME test COMMAND test)

# Build examples
foreach (example IN LISTS OPTIONPP_EXAMPLES)
  get_filename_component (CURRENT_EXAMPLE "${example}" NAME_WE)
  set (CURRENT_EXAMPLE "example_${CURRENT_EXAMPLE}")
  add_executable (
    ${CURRENT_EXAMPLE}
    "${example}"
    )
  target_include_directories (${CURRENT_EXAMPLE} PRIVATE single_header)
endforeach ()

# Set max warning level
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(optionpp PRIVATE -Wall -Wextra -pedantic)
  target_compile_options(test PRIVATE -Wall -Wextra -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(optionpp PRIVATE /W4)
  target_compile_options(test PRIVATE /W4)
endif ()
