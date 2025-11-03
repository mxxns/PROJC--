#include "cpu.h"
#include <sstream>
#include <fstream>
#include <string>

double Instruction::compute() {
    switch (opcode) {
        case ADD:
            return operand_l + operand_r;
        case SUB:
            return operand_l - operand_r;
        case MUL:
            return operand_l * operand_r;
        case DIV:
            if (operand_r != 0.0) {
                return operand_l / operand_r;
            } else {
                std::cerr << "Error: Division by zero." << std::endl;
                return 0.0;
            }
        case NOP:
            return 0.0;
    }
}

void Program::load(const std::string &filename) {
    instructions.clear();
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open program file: " << filename << std::endl;
        return;
    }

    std::string opcode_str;
    OPCODE opcode;
    double op_l, op_r;

    while (file >> opcode_str >> op_l >> op_r) {
        if (opcode_str == "ADD")
            opcode = ADD;
        else if (opcode_str == "SUB")
            opcode = SUB;
        else if (opcode_str == "MUL")
            opcode = MUL;
        else if (opcode_str == "DIV")
            opcode = DIV;
        else
            opcode = NOP;
        
        instructions.emplace_back(opcode, op_l, op_r); // Ajouter l'instruction à la fin de la liste
    }

    reset();
}

Instruction Program::compute() {
    if (pc == instructions.end()) {
        reset();
        return Instruction(NOP);
    }
    else {
        Instruction instr = *pc;
        ++pc;
        return instr;
    }
}

bool CPU::loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << filename << std::endl;
            return false;
        }
        std::string line, key, value, type;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            if (std::getline(iss, key, ':') && std::getline(iss, value)) {
                key = trim(key);
                value = trim(value);
                if (key == "TYPE") {
                    if (value != "CPU") {
                        std::cerr << "Error: TYPE must be 'CPU', found '" << value << "' instead." << std::endl;
                        return false;
                    }
                }
                else if (key == "LABEL") setLabel(value);
                else if (key == "N_CORES") setNCores(stoi(value));
                else if (key == "FREQUENCY") setFrequency(stoi(value));
            }
        }
        return true;
}

DataValue Register::pop() {
    if (fifo.empty()) {
        std::cerr << "Error: Attempt to pop from an empty register." << std::endl;
        return DataValue();
    }
    else{
        DataValue val = fifo.front();
        fifo.erase(fifo.begin());
        return val;
    }
}

void CPU::simulate() {
    for (int i = 0; i < frequency; ++i) {
        Instruction instr = program.compute();
        if (instr.opcode != NOP) {
            double result = instr.compute();
            registers.push(DataValue(result, true));
        }
        else {
            if (active_core >= n_cores - 1) {
                active_core = 0;
                program.reset();
                break;
            } else {
                ++active_core;
            }
        }
    }
}