//
// Created by arti1208 on 28.10.2020.
//

#ifndef ISP_DZ4_PROCESSOR_H
#define ISP_DZ4_PROCESSOR_H

#include <vector>
#include <string>
#include "assembler.h"

#define STACK_EL_TYPE double
#define STACK_EL_TO_STRING(v) std::to_string(v).c_str()

#include "unbreakable_stack.h"

#undef STACK_EL_TYPE
#undef STACK_EL_TO_STRING

#define STACK_EL_TYPE long
#define STACK_EL_TO_STRING(v) std::to_string(v).c_str()

#include "unbreakable_stack.h"

#undef STACK_EL_TYPE
#undef STACK_EL_TO_STRING

enum processor_state {
    PROCESSOR_OK,
    STACK_ERROR,
    ARITHMETIC_ERROR,
    IO_ERROR,
    JMP_ERROR,
    CALL_ERROR,
};

class processor {

private:
    unbr_stack_double* cpu_stack;
    unbr_stack_long* return_stack;
    double ax;
    double bx;
    double cx;
    double dx;
    long ip;

public:
    processor_state execute(const char* path);

    processor() {
        cpu_stack = new unbr_stack_double();
        return_stack = new unbr_stack_long();
        ax = bx = cx = dx = 0;
        ip = -1;
    }

    ~processor() {
        delete cpu_stack;
        delete return_stack;
    }

private:
    processor_state add();

    processor_state sub();

    processor_state mul();

    processor_state div();

    processor_state sqrt();

    processor_state in();

    processor_state out();

    processor_state pop();

    processor_state push(double value);

    processor_state popReg(char reg);

    processor_state pushReg(char reg);

    processor_state jmp(long position);

    processor_state ife(long position);

    processor_state ifl(long position);

    processor_state ifg(long position);

    processor_state ifne(long position);

    processor_state ifle(long position);

    processor_state ifge(long position);

    processor_state call(long position);

    processor_state ret();
};


#endif //ISP_DZ4_PROCESSOR_H
