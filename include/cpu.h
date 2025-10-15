#ifndef CPU_H__
#define CPU_H__

#include "lib.h"



enum OPCODE {
    NOP,
    ADD,
    SUB,
    MUL,
    DIV
};

struct Instruction {
    private:
        OPCODE opcode;
        int operand;
};

struct Program {

    Instruction* instructions;
    Instruction compute();
    void reset();
};

struct Register {

};

struct CPU : public ReadableComponent {

};

#endif