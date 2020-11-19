//
// Created by arti1208 on 28.10.2020.
//

#include <fcntl.h>
#include <libnet.h>
#include <sys/mman.h>
#include "assembler.h"

static size_t nextLexemLen(const char* str, size_t dataLen, size_t& from) {
    bool lexemStarted = false;
    for (size_t i = 0; i < dataLen; ++i) {
        if (i == dataLen || str[i] == '\n' || str[i] == ' ') {
            if (lexemStarted) {
                return i - from;
            }
        } else if (!lexemStarted) {
            from = i;
            lexemStarted = true;
        }
    }

    return dataLen - from;
}

static char readReg(const char* str, size_t len) {
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

bool assembler::readDouble(const char* str, size_t len, std::vector<char>& result) {
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

    pushIf(result, value, currentPass > 0);

    return true;
}

size_t assembler::readCommand(const char* str, size_t dataLen, std::vector<char>& result) {
    if (dataLen == 0) return 1;
    size_t commandStart = 0;
    size_t commandLen = nextLexemLen(str, dataLen, commandStart);

    str += commandStart;

    if (commandLen == 0 || dataLen < commandLen) return 0;

    if (str[commandLen - 1] == ':') { // label
        if (commandLen == 1) return 0;

        std::string d(str, commandLen - 1);

        if (labels.find(d) == labels.end()) {
            labels.emplace(d, bytesProcessed);
        }

        return commandStart + commandLen;
    }

    bool labelsSearch = currentPass == 0;

    if (commandLen == 4) {
        if (strncmp(str, "sqrt", commandLen) == 0) {
            pushIf(result, SQRT, !labelsSearch);
        } else if (strncmp(str, "push", commandLen) == 0) {
            size_t from = 0;
            size_t lexemLen = nextLexemLen(str + commandLen, dataLen - commandLen, from);

            char reg = readReg(str + commandLen + from, lexemLen);
            if (reg >= 0) {
                pushIf(result, PUSH_REG, !labelsSearch);
                pushIf(result, reg, !labelsSearch);
            } else {
                pushIf(result, PUSH, !labelsSearch);
                readDouble(str + commandLen + from, lexemLen, result);
            }

            return commandStart + commandLen + from + lexemLen;
        } else if (strncmp(str, "call", commandLen) == 0) {
            size_t labelProceed = processLabelCommand(CALL, str, commandLen, dataLen, result);
            if (labelProceed == 0) return 0;
            return commandStart + commandLen + labelProceed;
        } else if (strncmp(str, "ifle", commandLen) == 0) {
            size_t labelProceed = processLabelCommand(IFLE, str, commandLen, dataLen, result);
            if (labelProceed == 0) return 0;
            return commandStart + commandLen + labelProceed;
        } else if (strncmp(str, "ifge", commandLen) == 0) {
            size_t labelProceed = processLabelCommand(IFGE, str, commandLen, dataLen, result);
            if (labelProceed == 0) return 0;
            return commandStart + commandLen + labelProceed;
        } else if (strncmp(str, "ifne", commandLen) == 0) {
            size_t labelProceed = processLabelCommand(IFNE, str, commandLen, dataLen, result);
            if (labelProceed == 0) return 0;
            return commandStart + commandLen + labelProceed;
        }

        return commandStart + commandLen;
    }

    if (commandLen == 3) {
        command c;
        if (strncmp(str, "hlt", commandLen) == 0) {
            c = HLT;
        } else if (strncmp(str, "add", commandLen) == 0) {
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
                pushIf(result, POP_REG, !labelsSearch);
                pushIf(result, reg, !labelsSearch);
                return commandStart + commandLen + from + lexemLen;
            }

            return commandStart + commandLen;

        } else if (strncmp(str, "out", commandLen) == 0) {
            c = OUT;
        } else if (strncmp(str, "jmp", commandLen) == 0) {
            size_t labelProceed = processLabelCommand(JMP, str, commandLen, dataLen, result);

            if (labelProceed == 0) return 0;

            return commandStart + commandLen + labelProceed;
        } else if (strncmp(str, "ifl", commandLen) == 0) {
            size_t labelProceed = processLabelCommand(IFL, str, commandLen, dataLen, result);
            if (labelProceed == 0) return 0;
            return commandStart + commandLen + labelProceed;
        } else if (strncmp(str, "ifg", commandLen) == 0) {
            size_t labelProceed = processLabelCommand(IFG, str, commandLen, dataLen, result);
            if (labelProceed == 0) return 0;
            return commandStart + commandLen + labelProceed;
        } else if (strncmp(str, "ife", commandLen) == 0) {
            size_t labelProceed = processLabelCommand(IFE, str, commandLen, dataLen, result);
            if (labelProceed == 0) return 0;
            return commandStart + commandLen + labelProceed;
        } else if (strncmp(str, "ret", commandLen) == 0) {
            c = RET;
        } else return commandStart + commandLen;

        pushIf(result, c, !labelsSearch);

        return commandStart + commandLen;
    }

    if (commandLen == 2) {
        command c;
        if (strncmp(str, "in", commandLen) == 0) {
            c = IN;
        } else return commandStart + commandLen;

        pushIf(result, c, !labelsSearch);

        return commandStart + commandLen;
    }


    char* cmd = new char[commandLen];
    strncpy(cmd, str, commandLen);

    printf("Unknown cmd near '%s' of len %ld\n", cmd, commandLen);

    delete[] cmd;

    return 0;
}

bool assembler::assemble(const char* path, std::vector<char>& result) {

    int descriptor = open(path, O_RDONLY);

    struct stat fileStat{};
    if (fstat(descriptor, &fileStat) == -1) return false;

    char* data = (char*) mmap(nullptr, fileStat.st_size, PROT_READ, MAP_PRIVATE, descriptor, 0);
    if (data == MAP_FAILED) return false;

    close(descriptor);

    for (uint i = 0; i < codePasses; ++i) {
        bool codePassResult = runCodePass(data, fileStat.st_size, i, result);
        if (!codePassResult) {
            munmap(data, fileStat.st_size);
            return false;
        }
    }

    munmap(data, fileStat.st_size);

    return true;
}

bool assembler::runCodePass(const char* code, off_t codeLength, uint passNumber, std::vector<char>& result) {
    currentPass = passNumber;
    bytesProcessed = 0;

    off_t start = 0;
    size_t lastCommandIndex = 0;
    for (off_t i = 0; i <= codeLength;) {
        if (i == codeLength || code[i] == '\n' || code[i] == ' ') {
            size_t commandReadResult = readCommand(code + start, codeLength - start, result);
            if (commandReadResult == 0) {
                return false;
            }

            if (code[start + commandReadResult - 1] != ':') {
                ++lastCommandIndex;
            }

            i = start + commandReadResult + 1;
            start = i;
        } else {
            ++i;
        }
    }

    return true;
}

size_t assembler::processLabelCommand(command ifCommand, const char* str, size_t commandLen, size_t dataLen,
                                      std::vector<char>& result) {
    size_t from = 0;
    size_t lexemLen = nextLexemLen(str + commandLen, dataLen - commandLen, from);

    std::string jmpLabel(str + commandLen + from, lexemLen);

    bool labelsSearch = currentPass == 0;

    auto iter = labels.find(jmpLabel);
    if (iter == labels.end()) {

        if (!labelsSearch) {
            printf("Not Found jmp label '%s'\n", jmpLabel.c_str());
            return 0;
        }

        bytesProcessed += sizeof(char) + sizeof(long);
    } else {
        pushIf(result, ifCommand, !labelsSearch);
        pushIf(result, iter->second, !labelsSearch);

    }

    return from + lexemLen;
}
