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
    private:
        std::vector<DataValue> fifo;
    public:
        Register() = default;

        // push a value to the back of the FIFO
        void push(const DataValue &v) { fifo.push_back(v); }
        void push(DataValue &&v) { fifo.push_back(std::move(v)); }

        // try to pop the front value; returns false if empty
        bool try_pop(DataValue &out) {
            if (fifo.empty()) return false;
            out = fifo.front();
            fifo.erase(fifo.begin());
            return true;
        }

        // pop and return the front value; returns a default-constructed DataValue if empty
        DataValue pop() {
            if (fifo.empty()) return DataValue();
            DataValue v = fifo.front();
            fifo.erase(fifo.begin());
            return v;
        }

        // peek at the front element (nullptr if empty)
        const DataValue* peek() const {
            return fifo.empty() ? nullptr : &fifo.front();
        }

        size_t size() const { return fifo.size(); }
        bool empty() const { return fifo.empty(); }
        void clear() { fifo.clear(); }
};

struct CPU : public ReadableComponent {

};

#endif