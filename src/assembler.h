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

enum mode : int {
    WAITING_COMMAND = 0x100,
    WAITING_REG = 0x1000,
    WAITING_NUM = 0x10000,
    WAITING_REG_OR_NUM = 0x100000,
};

bool assembly(const char* path, std::vector<char>& result);


#endif //ISP_DZ4_ASSEMBLER_H
