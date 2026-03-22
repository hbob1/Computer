#include "include/assembler.h"
#include <iomanip>
#include <iostream>

int main() {
    auto output = assemble("asm/program.asm");
    for (auto b : output) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
    }
    std::cout << std::endl;
    
}