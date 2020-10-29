cmake_minimum_required(VERSION 3.17)
project(ISP_DZ4)

set(CMAKE_CXX_STANDARD 20)

add_executable(executable
        main.cpp
        processor.cpp processor.h
        assembler.cpp assembler.h
        disassembler.cpp disassembler.h
        unbreakable_stack.h
        checks.h)

add_executable(cpu
        main_cpu.cpp
        processor.cpp processor.h
        assembler.cpp assembler.h
        unbreakable_stack.h
        checks.h)

add_executable(asm
        main_asm.cpp
        assembler.cpp assembler.h)

add_executable(disasm
        main_disasm.cpp
        disassembler.cpp disassembler.h)

target_compile_options(executable PRIVATE -Wall -Wextra)