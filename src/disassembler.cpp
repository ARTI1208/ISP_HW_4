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

size_t disassembler::readLabel(const char* str, size_t dataLen, std::string& command) {
    if (dataLen < sizeof(long)) return 0;

    union {
        char data[sizeof(long)];
        long value;
    } converter {};

    for (size_t i = 0; i < sizeof(long); ++i) {
        converter.data[i] = str[i];
    }

    auto iter = labels.find(converter.value);
    if (iter == labels.end()) {
        std::string labelName = "label_" + std::to_string(labels.size());
        labels.emplace(converter.value, labelName);
        command += labelName;
    } else {
        command += iter->second;
    }

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

size_t disassembler::readCommand(const char* str, size_t dataLen, std::string& command) {
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
        case HLT:      command = "hlt";      return sizeof(char);
        case RET:      command = "ret";      return sizeof(char);
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
        case JMP:
            command = "jmp ";
            if (readLabel(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(long);
        case CALL:
            command = "call ";
            if (readLabel(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(long);
        case IFE:
            command = "ife ";
            if (readLabel(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(long);
        case IFNE:
            command = "ifne ";
            if (readLabel(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(long);
        case IFL:
            command = "ifl ";
            if (readLabel(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(long);
        case IFLE:
            command = "ifle ";
            if (readLabel(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(long);
        case IFG:
            command = "ifg ";
            if (readLabel(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(long);
        case IFGE:
            command = "ifge ";
            if (readLabel(str + sizeof(char), dataLen - sizeof(char), command) == 0) return 0;
            return sizeof(char) + sizeof(long);
    }

    printf("Error: unknown command %d\n", str[0]);

    return 0;
}

bool disassembler::disassemble(const char* originalPath, const char* writePath) {
    int descriptor = open(originalPath, O_RDONLY);

    struct stat fileStat{};
    if (fstat(descriptor, &fileStat) == -1) return false;

    char* data = (char*) mmap(nullptr, fileStat.st_size, PROT_READ, MAP_PRIVATE, descriptor, 0);
    if (data == MAP_FAILED) return false;

    close(descriptor);

    for (uint i = 0; i < codePasses; ++i) {
        bool res = runCodePass(data, fileStat.st_size, i, writePath);

        if (!res) {
            munmap(data, fileStat.st_size);
            return false;
        }
    }

    munmap(data, fileStat.st_size);

    return true;
}

bool disassembler::runCodePass(const char* code, off_t codeLength, uint passNumber, const char* writePath) {

    if (passNumber == 0) { //decode labels

        for (off_t i = 0; i < codeLength;) {
            std::string command;
            size_t commandReadResult = readCommand(code + i, codeLength - i, command);
            if (commandReadResult == 0) return false;

            i += commandReadResult;
        }

        return true;
    } else if (passNumber == 1) { // full decode
        FILE* outputFile = fopen(writePath, "w");

        for (off_t i = 0; i < codeLength;) {
            std::string command;
            size_t commandReadResult = readCommand(code + i, codeLength - i, command);
            if (commandReadResult == 0) return false;

            fwrite(command.c_str(), sizeof(char), command.length(), outputFile);
            fprintf(outputFile, "%s", "\n");

            i += commandReadResult;

            auto iter = labels.find(i);
            if (iter != labels.end()) {
                fprintf(outputFile, "%s:\n", labels[i].data());
            }
        }

        fclose(outputFile);
        return true;
    }


    return false;
}
