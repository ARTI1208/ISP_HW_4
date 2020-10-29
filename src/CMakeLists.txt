cmake_minimum_required(VERSION 3.17)
project(ISP_DZ4)

set(CMAKE_CXX_STANDARD 20)

add_executable(executable
        main.cpp
        processor.cpp processor.h
        assembler.cpp assembler.h
        disassebmler.cpp disassebmler.h
        unbreakable_stack.h
        checks.h)

target_compile_options(executable PRIVATE -Wall -Wextra)