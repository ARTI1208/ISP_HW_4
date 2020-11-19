#include <cstring>
#include "assembler.h"

static int runAssemble(const char* programPath, const char* outputPath) {

    std::vector<char> assembledCode;

    assembler assembler;
    bool res = assembler.assemble(programPath, assembledCode);

    if (!res) {
        printf("Assemble error\n");
        return -1;
    }

    FILE* out = fopen(outputPath, "w");

    fwrite(assembledCode.data(), sizeof(char), assembledCode.size(), out);

    fclose(out);
    printf("Assemble successful\n");
    return 0;
}

static void printHelp() {
    printf("Assembler help note.\n");
    printf("    <input> <output>     -- assembles program written in pseudo-asm in the file <input> and writes result to file <output>\n");
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

    return runAssemble(argv[1], argv[2]);
}
