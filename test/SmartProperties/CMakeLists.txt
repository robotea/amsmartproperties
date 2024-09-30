cmake_minimum_required(VERSION 3.10)
set(${CMAKE_RUNTIME_LIBRARY_OUTPUT_DIRECTORY} "../../build/tests/SmartProperties")
project(smart_properties)

set(CMAKE_CXX_STANDARD 14)
include_directories(../../include ../../../3rdparty/include)
link_directories("../../../3rdparty/lib")

add_executable(smart_properties test_AMProperties.cpp ../src/AMCoords.cpp ../src/AMWindow.cpp ../src/AMView.cpp ../../src/AMProperty.cpp ../../src/AMVariableNames.cpp)
target_link_libraries(smart_properties libgtest.a pthread)
