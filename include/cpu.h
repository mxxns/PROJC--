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
    double operand_l;
    double operand_r;

public:
    OPCODE opcode;
    Instruction(OPCODE op = NOP, double l = 0.0, double r = 0.0)
        : opcode(op), operand_l(l), operand_r(r) {}

    double compute();  // implemented in cpu.cpp
};


struct Program {
private:
    std::list<Instruction> instructions;
    std::list<Instruction>::iterator pc = instructions.begin(); // program counter, pointant sur l'instruction courante

public:
    Instruction compute();       // implemented in cpu.cpp

    void load(const std::string &filename); //implemented in cpu.cpp
    
    void reset(){
        pc = instructions.begin();
    };
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
};

class CPU : public ReadableComponent {
    public:
        CPU(int freq = 1000, int n_cores = 1, const std::string &lbl = "") 
            : ReadableComponent(lbl), frequency(freq), n_cores(n_cores), active_core(0) {};

        ~CPU() override = default;

        DataValue read() override{
            return registers.pop();
        };

        bool loadFromFile(const std::string &filename) override;

        void loadProgram(const std::string &filename){
            program.load(filename);
        }

        void setFrequency(int freq){frequency = freq;}
        void setNCores(int n){n_cores = n;}
        void setActiveCore(int core){active_core = core;}

        void printInfo() const override;

        void simulate() override;  // definition de la methode virtuelle de component, implementee dans cpu.cpp

    private:
        int frequency;
        int n_cores;
        int active_core;
        Program program;
        Register registers;
};      

#endif
