cmake_minimum_required(VERSION 3.10)
set(${CMAKE_RUNTIME_LIBRARY_OUTPUT_DIRECTORY} "../../build/tests/Function")
project(function)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES test_AMFunction.cpp test_AMFunction2.cpp test_AMFunction3.cpp ../../AMFunction.h)
include_directories(../../include ../../../3rdparty/include)
link_directories("../../../3rdparty/lib" "../../build/bin/")

add_executable(function test_AMFunction.cpp test_AMFunction2.cpp test_AMFunction3.cpp)
target_link_libraries(function libgtest.a pthread AMSmartProperties)
