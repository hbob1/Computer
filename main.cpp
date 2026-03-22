#include "include/assembler.h"
#include "include/cpu.h"
#include "include/isa.h"
#include <bitset>
#include <iomanip>
#include <iostream>

static void print_trace(const CPU& cpu) {
    std::cout
        << "SC=" << std::setw(3) << static_cast<int>(cpu.SC)
        << " PC=" << std::setw(3) << static_cast<int>(cpu.PC)
        << " IR=0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.IR) << std::dec
        << " OP=" << std::setw(3) << static_cast<int>(cpu.operand)
        << " AC=" << std::setw(3) << static_cast<int>(cpu.AC)
        << " ALU=" << std::setw(3) << static_cast<int>(cpu.ALU)
        << " MR=" << std::setw(3) << static_cast<int>(cpu.MR)
        << " FLAGS=" << std::bitset<8>(cpu.FLAGS)
        << std::setfill(' ')
        << '\n';

    std::cout << "RAM[0..15]: ";
    for (size_t i = 0; i < 16 && i < cpu.RAM.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(cpu.RAM[i]) << ' ';
    }
    std::cout << std::dec << std::setfill(' ') << '\n';
}

int main() {
    auto output = assemble("asm/program.asm");
    std::cout << "Program bytes: ";
    for (auto b : output) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << std::setfill(' ') << "\n";

    CPU cpu{};
    cpu.RAM.assign(256, 0);

    for (size_t i = 0; i < output.size() && i < cpu.RAM.size(); ++i) {
        cpu.RAM[i] = output[i];
    }

    constexpr int max_steps = 100;
    for (int i = 0; i < max_steps && !cpu.halted; ++i) {
        step(cpu);
        print_trace(cpu);
    }

    if (!cpu.halted) {
        std::cout << "Execution stopped after " << max_steps << " steps (likely infinite loop).\n";
    }

    return 0;
}
