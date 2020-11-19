//
// Created by arti1208 on 28.10.2020.
//

#ifndef ISP_DZ4_DISASSEMBLER_H
#define ISP_DZ4_DISASSEMBLER_H


#include <vector>
#include <string>
#include <unordered_map>

class disassembler {
private:
    std::unordered_map<long, std::string> labels;

    const uint codePasses = 2;
    uint currentPass = 0;

    size_t readLabel(const char* str, size_t dataLen, std::string& command);

    size_t readCommand(const char* str, size_t dataLen, std::string& command);

    bool runCodePass(const char* code, off_t codeLength, uint passNumber, const char* writePath);

public:
    bool disassemble(const char* originalPath, const char* writePath);
};


#endif //ISP_DZ4_DISASSEMBLER_H
