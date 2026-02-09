#include "../include/assembler.h"
#include <cstdint>
#include <fstream>
#include <iostream>

std::vector<uint8_t> assemble(const std::string& filename) {
    std::ifstream file;
    file.open(filename);

    std::vector<uint8_t> result;

    if(file.is_open()) {
        std::string line;

        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
    }

    return result;
}


