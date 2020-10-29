#include "processor.h"

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

static void printHelp() {
    printf("CPU emulator help note.\n");
    printf("    <filename>           -- executes program written in pseudo-asm in the file <filename>\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Malformed request, checkout help:\n");
        printHelp();
        return -1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printHelp();
        return 0;
    }

    return runExecute(argv[1]);
}
