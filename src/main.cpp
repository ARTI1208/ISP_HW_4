#include <iostream>
#include "processor.h"

int main(int argc, char* argv[]) {

    processor p;

    if (argc > 1) {
        p.execute(argv[1]);
    } else {
        std::string path;
        std::cin >> path;
        processor_state res = p.execute(path.c_str());
        if (res == PROCESSOR_OK) {
            printf("Execution successful\n");
        } else {
            printf("Execution error: %d\n", res);
        }
    }

    return 0;
}
