cmake_minimum_required(VERSION 3.17)
project(ISP_DZ4)

set(CMAKE_CXX_STANDARD 20)

add_executable(executable main.cpp)

target_compile_options(executable PRIVATE -Wall -Wextra)