//
// Created by arti1208 on 28.10.2020.
//

#include <fstream>
#include <fcntl.h>
#include <libnet.h>
#include <sys/mman.h>
#include <iostream>
#include "assembler.h"

static int commandParamsCount(command c) {
    switch (c) {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case SQRT:
        case IN:
        case OUT:
        case POP:
        case PUSH: return 1 | WAITING_NUM;
        case POP_REG: return 1 | WAITING_REG;
        case PUSH_REG: return 2 | WAITING_REG;
    }

    return 0;
}

static size_t nextLexemLen(const char* str, size_t dataLen, size_t& from) {
    bool lexemStarted = false;
    for (size_t i = 0; i < dataLen; ++i) {
//        std::cout << str[i];
        if (i == dataLen || str[i] == '\n' || str[i] == ' ') {
            if (lexemStarted) {
//                std::cout << "|";
                return i - from;
            }
        } else if (!lexemStarted){
            from = i;
            lexemStarted = true;
        }
    }

//    std::cout << "|";
    return dataLen;
}

static char readReg(char* str, size_t len) {
    if (len != 2) return -1;

    if (strncmp(str, "ax", len) == 0) {
        return 0;
    } else if (strncmp(str, "bx", len) == 0) {
        return 1;
    } else if (strncmp(str, "cx", len) == 0) {
        return 2;
    } else if (strncmp(str, "dx", len) == 0) {
        return 3;
    } else return -1;
}

static bool readRegAndPush(char* str, size_t len, std::vector<char>& result) {
    char reg = readReg(str, len);
    if (reg < 0) return false;

    result.push_back(reg);

    return true;
}

static bool readNum(char* str, size_t len, std::vector<char>& result) {
    union {
        char data[sizeof(double)];
        double value;
    } converter {};

    char* substr = new char[len + 1];
    substr[len] = '\0';

    strncpy(substr, str, len);

    char* end;
    double value = std::strtod(substr, &end);

    if (substr + len != end) {
        printf("convert error: expected %p, actual %p, conv %lf", substr + len, end, value);
        delete[] substr;
        return false;
    }

    delete[] substr;

    converter.value = value;

    for (char& i : converter.data) {
        result.push_back(i);
    }

    return true;
}

static size_t readCommand(char* str, size_t dataLen, std::vector<char>& result) {
    size_t commandStart = 0;
    size_t commandLen = nextLexemLen(str, dataLen, commandStart);

    str += commandStart;

    if (commandLen == 0 || dataLen < commandLen) return dataLen;

    if (commandLen == 4) {
        if (strncmp(str, "sqrt", commandLen) == 0) {
            result.push_back(SQRT);
        } else if (strncmp(str, "push", commandLen) == 0) {
            size_t from = 0;
            size_t lexemLen = nextLexemLen(str + commandLen, dataLen - commandLen, from);

            char reg = readReg(str + commandLen + from, lexemLen);
            if (reg >= 0) {
                result.push_back(PUSH_REG);
                result.push_back(reg);
            } else {
                result.push_back(PUSH);
                readNum(str + commandLen + from, lexemLen, result);
            }

            return commandStart + commandLen + from + lexemLen;
        }

        return commandStart + commandLen;
    }

    if (commandLen == 3) {
        command c;
        if (strncmp(str, "add", commandLen) == 0) {
            c = ADD;
        } else if (strncmp(str, "sub", commandLen) == 0) {
            c = SUB;
        } else if (strncmp(str, "mul", commandLen) == 0) {
            c = MUL;
        } else if (strncmp(str, "div", commandLen) == 0) {
            c = DIV;
        } else if (strncmp(str, "pop", commandLen) == 0) {

            size_t from = 0;
            size_t lexemLen = nextLexemLen(str + commandLen, dataLen - commandLen, from);

            char reg = readReg(str + commandLen + from, lexemLen);
            if (reg >= 0) {
                result.push_back(POP_REG);
                result.push_back(reg);
                return commandStart + commandLen + from + lexemLen;
            }

            return commandStart + commandLen;

        } else if (strncmp(str, "out", commandLen) == 0) {
            c = OUT;
        } else return commandStart + commandLen;

        result.push_back(c);

        return commandStart + commandLen;
    }

    if (commandLen == 2) {
        command c;
        if (strncmp(str, "in", commandLen) == 0) {
            c = IN;
        } else return commandStart + commandLen;

        result.push_back(c);

        return commandStart + commandLen;
    }


    return -1;
}

bool assembly(const char* path, std::vector<char>& result) {

    int descriptor = open(path, O_RDONLY);

    struct stat fileStat{};
    if (fstat(descriptor, &fileStat) == -1) return false;

    char* data = (char*) mmap(nullptr, fileStat.st_size, PROT_READ, MAP_PRIVATE, descriptor, 0);
    if (data == MAP_FAILED) return false;

    close(descriptor);

    off_t start = 0;
    for (off_t i = 0; i <= fileStat.st_size;) {
        if (i == fileStat.st_size || data[i] == '\n' || data[i] == ' ') {
            i = start + readCommand(data + start, fileStat.st_size - start, result) + 1;
            start = i;
        } else {
            ++i;
        }
    }

    munmap(data, fileStat.st_size);

    return true;
}