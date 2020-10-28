cmake_minimum_required(VERSION 3.17)
project(ISP_DZ4)

set(CMAKE_CXX_STANDARD 20)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)

add_dependencies(tests executable)