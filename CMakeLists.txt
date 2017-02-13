cmake_minimum_required (VERSION 2.8)
project (option++)

file(GLOB SOURCES "src/*.cpp")

add_executable(option++ ${SOURCES})
