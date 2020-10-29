//
// Created by arti1208 on 28.10.2020.
//

#include <cmath>
#include <iostream>
#include "processor.h"
#include "assembler.h"
#include "checks.h"

static bool charsToDouble(const std::vector<char>& data, size_t from, double& res) {
    if (from + sizeof(double) >= data.size()) return false;
    memcpy(&res, data.data() + from, sizeof(double));

    return true;
}

processor_state processor::execute(const char* path) {
    std::vector<char> data;
    if (!assemble(path, data)) return IO_ERROR;

    for (size_t i = 0; i < data.size();) {
        processor_state state = PROCESSOR_OK;
        switch (data[i]) {
            case ADD:      state = add();              break;
            case SUB:      state = sub();              break;
            case MUL:      state = mul();              break;
            case DIV:      state = div();              break;
            case SQRT:     state = sqrt();             break;
            case IN:       state = in();               break;
            case OUT:      state = out();              break;
            case POP:      state = pop();              break;
            case PUSH: {
                double v;
                bool convertResult = charsToDouble(data, i + 1, v);
                if (!convertResult) return IO_ERROR;
                i += sizeof(double);
                state = push(v);
                break;
            }
            case POP_REG:  state = popReg(data[++i]);           break;
            case PUSH_REG: state = pushReg(data[++i]);          break;


            default: break;
        }

        if (state != PROCESSOR_OK) return state;

        ++i;
    }


    return PROCESSOR_OK;
}

processor_state processor::add() {
    double firstElem = NAN;
    double secondElem = NAN;

    unbr_stack_state firstPop = cpu_stack->pop(firstElem);
    if (firstPop != OK) return STACK_ERROR;

    unbr_stack_state secondPop = cpu_stack->pop(secondElem);
    if (secondPop != OK) return STACK_ERROR;

    return cpu_stack->push(firstElem + secondElem) == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::sub() {
    double firstElem = NAN;
    double secondElem = NAN;

    unbr_stack_state firstPop = cpu_stack->pop(firstElem);
    if (firstPop != OK) return STACK_ERROR;

    unbr_stack_state secondPop = cpu_stack->pop(secondElem);
    if (secondPop != OK) return STACK_ERROR;

    return cpu_stack->push(secondElem - firstElem) == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::mul() {
    double firstElem = NAN;
    double secondElem = NAN;

    unbr_stack_state firstPop = cpu_stack->pop(firstElem);
    if (firstPop != OK) return STACK_ERROR;

    unbr_stack_state secondPop = cpu_stack->pop(secondElem);
    if (secondPop != OK) return STACK_ERROR;

    return cpu_stack->push(firstElem * secondElem) == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::div() {
    double firstElem = NAN;
    double secondElem = NAN;

    unbr_stack_state firstPop = cpu_stack->pop(firstElem);
    if (firstPop != OK) return STACK_ERROR;

    unbr_stack_state secondPop = cpu_stack->pop(secondElem);
    if (secondPop != OK) return STACK_ERROR;

    if (isZero(secondElem)) return ARITHMETIC_ERROR;

    return cpu_stack->push(firstElem / secondElem) == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::sqrt() {
    double elem = NAN;

    unbr_stack_state popResult = cpu_stack->pop(elem);
    if (popResult != OK) return STACK_ERROR;

    return cpu_stack->push(std::sqrt(elem)) == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::in() {
    printf("in: ");
    double v;
    std::cin >> v;
    printf("\n");
    return cpu_stack->push(v) == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::pop() {
    double tmp;

    return cpu_stack->pop(tmp) == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::out() {
    double v = NAN;
    unbr_stack_state popResult = cpu_stack->pop(v);
    if (popResult != OK) return STACK_ERROR;

    printf("out: %lf\n", v);

    return PROCESSOR_OK;
}

processor_state processor::push(double value) {
    return cpu_stack->push(value) == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::popReg(char reg) {
    unbr_stack_state stackState;
    switch (reg) {
        case 0: stackState = cpu_stack->pop(ax); break;
        case 1: stackState = cpu_stack->pop(bx); break;
        case 2: stackState = cpu_stack->pop(cx); break;
        case 3: stackState = cpu_stack->pop(dx); break;
        default: return IO_ERROR;
    }

    return stackState == OK ? PROCESSOR_OK : STACK_ERROR;
}

processor_state processor::pushReg(char reg) {
    unbr_stack_state stackState;
    switch (reg) {
        case 0: stackState = cpu_stack->push(ax); break;
        case 1: stackState = cpu_stack->push(bx); break;
        case 2: stackState = cpu_stack->push(cx); break;
        case 3: stackState = cpu_stack->push(dx); break;
        default: return IO_ERROR;
    }

    return stackState == OK ? PROCESSOR_OK : STACK_ERROR;
}
