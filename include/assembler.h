#pragma once
#include <cstdint>
#include <string>
#include <vector>

std::vector<uint8_t> assemble(const std::string& filename);
int instruction_size(const std::vector<std::string>& tokens);