#include <cstring>
#include <cstdio>
#include "disassembler.h"

static int runDisassemble(const char* bytesPath, const char* outputPath) {

    disassembler disassembler;
    bool res = disassembler.disassemble(bytesPath, outputPath);

    if (res) {
        printf("Disassemble successful\n");
    } else {
        printf("Disassemble error\n");
    }

    return res ? 0 : -1;
}

static void printHelp() {
    printf("Assembler help note.\n");
    printf("    <input> <output>     -- disassembles program bytecode in the file <input> and writes result to file <output>\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Malformed request, checkout help:\n");
        printHelp();
        return -1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printHelp();
        return 0;
    }

    return runDisassemble(argv[1], argv[2]);
}
