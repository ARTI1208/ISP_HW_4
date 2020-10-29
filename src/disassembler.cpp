//
// Created by arti1208 on 28.10.2020.
//

#include <fcntl.h>
#include <libnet.h>
#include <sys/mman.h>
#include <string>
#include "assembler.h"
#include "disassembler.h"

static size_t readNum(const char* str, size_t dataLen, std::string& command) {
    if (dataLen < sizeof(double)) return 0;

    union {
        char data[sizeof(double)];
        double value;
    } converter {};

    for (size_t i = 0; i < sizeof(double); ++i) {
        converter.data[i] = str[i];
    }

    command += std::to_string(converter.value);

    return sizeof(double);
}

static size_t readReg(const char* str, size_t dataLen, std::string& command) {
    if (dataLen < sizeof(char)) return 0;

    switch (str[0]) {
        case 0: command += "ax"; break;
        case 1: command += "bx"; break;
        case 2: command += "cx"; break;
        case 3: command += "dx"; break;
        default: return 0;
    }

    return sizeof(char);
}

static size_t readCommand(const char* str, size_t dataLen, std::string& command) {
    if (dataLen == 0) return 0;

    switch (str[0]) {
        case ADD:      command = "add";      return sizeof(char);
        case SUB:      command = "sub";      return sizeof(char);
        case MUL:      command = "mul";      return sizeof(char);
        case DIV:      command = "div";      return sizeof(char);
        case SQRT:     command = "sqrt";     return sizeof(char);
        case IN:       command = "in";       return sizeof(char);
        case OUT:      command = "out";      return sizeof(char);
        case POP:      command = "pop";      return sizeof(char);
        case PUSH:
            command = "push ";
            if (readNum(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(double);
        case POP_REG:
            command = "pop ";
            if (readReg(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return 2 * sizeof(char);
        case PUSH_REG:
            command = "push ";
            if (readReg(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return 2 * sizeof(char);
    }

    return 0;
}

bool disassemble(const char* originalPath, const char* writePath) {
    int descriptor = open(originalPath, O_RDONLY);

    struct stat fileStat{};
    if (fstat(descriptor, &fileStat) == -1) return false;

    char* data = (char*) mmap(nullptr, fileStat.st_size, PROT_READ, MAP_PRIVATE, descriptor, 0);
    if (data == MAP_FAILED) return false;

    close(descriptor);


    FILE* outputFile = fopen(writePath, "w");

    for (off_t i = 0; i <= fileStat.st_size;) {
        std::string command;
        size_t commandReadResult = readCommand(data + i, fileStat.st_size - i, command);
        if (commandReadResult == 0) return false;

        fwrite(command.c_str(), sizeof(char), command.length(), outputFile);
        fprintf(outputFile, "%s", "\n");

        i += commandReadResult;
    }

    fclose(outputFile);
    munmap(data, fileStat.st_size);

    return true;
}