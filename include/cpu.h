#ifndef CPU_H__
#define CPU_H__

#include "lib.h"
#include <vector>
#include <list>

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
    double operand_l;
    double operand_r;
public:
    Instruction(OPCODE op = NOP, double l = 0.0, double r = 0.0)
        : opcode(op), operand_l(l), operand_r(r) {}

    double compute();  // implemented in cpu.cpp
};


struct Program {
    std::list<Instruction> instructions;

    // Cette implementation de compute ne calcule qu'une instruction à la fois, c'est au cpu de relancer compute
    double compute() {
        double result = instructions.front().compute();
        instructions.pop_front();
        return result;
    }
    
    void reset();           // implemented in cpu.cpp
};


struct Register {
private:
    std::vector<DataValue> fifo;
public:
    Register() = default;

    DataValue pop();        // implemented in cpu.cpp

    // Definition des methodes utiles quand on travaille avec un FIFO. Definitions simples donc dans le header

    void push(const DataValue &v) {
        fifo.push_back(v);
    }

    const DataValue* peek() const {
        return fifo.empty() ? nullptr : &fifo.front();
    }

    size_t size() const { 
        return fifo.size(); 
    }
    
    bool empty() const {
        return fifo.empty(); 
    }
    
    void clear() {
        fifo.clear();
    }
};

struct CPU : public ReadableComponent {
    
};

#endif
