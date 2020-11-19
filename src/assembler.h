//
// Created by arti1208 on 28.10.2020.
//

#ifndef ISP_DZ4_ASSEMBLER_H
#define ISP_DZ4_ASSEMBLER_H

#include <vector>
#include <string>
#include <unordered_map>

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
    JMP,
    IFL,
    IFLE,
    IFG,
    IFGE,
    IFE,
    IFNE,
    CALL,
    RET,
    HLT
};

class assembler {
private:
    std::unordered_map<std::string, long> labels;

    const uint codePasses = 2;
    uint currentPass = 0;

    size_t bytesProcessed = 0;

    template<class V>
    void pushValue(std::vector<char>& result, V value) {
        union {
            char data[sizeof(V)];
            V value;
        } converter{};

        converter.value = value;

        for (char& i : converter.data) {
            result.push_back(i);
        }
    }

    template<class V>
    inline void pushIf(std::vector<char>& result, V value, bool condition) {
        if (condition) pushValue(result, value);
        bytesProcessed += sizeof(V);
    }

    static inline void pushValue(std::vector<char>& result, char value) {
        result.push_back(value);
    }

    inline void pushIf(std::vector<char>& result, char value, bool condition) {
        if (condition) pushValue(result, value);
        bytesProcessed += sizeof(char);
    }

    bool readDouble(const char* str, size_t len, std::vector<char>& result);

    bool runCodePass(const char* code, off_t codeLength, uint passNumber, std::vector<char>& result);

    size_t processLabelCommand(command ifCommand, const char* str, size_t commandLen, size_t dataLen,
                               std::vector<char>& result);


    size_t readCommand(const char* str, size_t dataLen, std::vector<char>& result);

public:

    bool assemble(const char* path, std::vector<char>& result);
};


#endif //ISP_DZ4_ASSEMBLER_H
