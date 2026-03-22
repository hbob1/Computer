#pragma once
#include <cstdint>
#include <vector>

struct CPU {
    uint8_t AC;
    uint8_t PC;
    uint8_t SP;
    uint8_t MR;
    uint8_t SC;      //SC + IR + operand = Instruction Register
    uint8_t IR; 
    uint8_t operand;
    uint8_t FLAGS;
    uint8_t ALU;     // last ALU result

    std::vector<uint8_t> RAM;

    bool halted;
    bool pc_overridden;
};

void step(CPU& cpu);

void controlLogic(CPU& cpu);
