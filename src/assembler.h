//
// Created by arti1208 on 28.10.2020.
//

#ifndef ISP_DZ4_ASSEMBLER_H
#define ISP_DZ4_ASSEMBLER_H

#include <vector>
#include <string>

enum command : char {
    ADD,
    SUB,
    MUL,
    DIV,
    SQRT,
    IN,
    OUT,
    POP,
    PUSH,
    POP_REG,
    PUSH_REG,
};

bool assemble(const char* path, std::vector<char>& result);


#endif //ISP_DZ4_ASSEMBLER_H
