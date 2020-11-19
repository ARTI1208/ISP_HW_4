#include "processor.h"
#include "assembler.h"
#include "disassembler.h"

static int runExecute(const char* programPath) {
    processor p;
    processor_state res = p.execute(programPath);
    if (res == PROCESSOR_OK) {
        printf("Execution successful\n");
        return 0;
    } else {
        printf("Execution error: %d\n", res);
        return res;
    }
}

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
    printf("CPU emulator help note.\n");
    printf("    e <filename>           -- executes program written in pseudo-asm in the file <filename>\n");
    printf("    a <input> <output>     -- assembles program written in pseudo-asm in the file <input> and writes result to file <output>\n");
    printf("    d <input> <output>     -- disassembles program bytecode in the file <input> and writes result to file <output>\n");
}

int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printHelp();
        return 0;
    }

    if (strcmp(argv[1], "e") == 0 && argc > 2) return runExecute(argv[2]);
    else if (strcmp(argv[1], "a") == 0 && argc > 3) return runAssemble(argv[2], argv[3]);
    else if (strcmp(argv[1], "d") == 0 && argc > 3) return runDisassemble(argv[2], argv[3]);

    printf("Malformed request, checkout help:\n");
    printHelp();

    return -1;
}
