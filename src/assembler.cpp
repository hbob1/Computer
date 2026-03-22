#include "../include/assembler.h"
#include "../include/isa.h"
#include <cstdint>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>

std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

int instruction_size(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return 0;

    std::string opcode = tokens[0];
    if (opcode == "nop") return 1;
    else if (opcode == "lda") return 2;
    else if (opcode == "sta") return 2;
    else if (opcode == "ina") return 1;
    else if (opcode == "cmp") return 1;
    else if (opcode == "jmp") return 2;
    else if (opcode == "beq") return 2;
    else if (opcode == "hlt") return 1;

    return 0;
}

static uint8_t parse_operand(
    const std::string& s,
    const std::unordered_map<std::string,uint8_t>& labels)
{
    if (std::isdigit(s[0]))
        return static_cast<uint8_t>(std::stoi(s));

    return labels.at(s);
}

static void assemble_instruction(
    const std::vector<std::string>& t,
    const std::unordered_map<std::string,uint8_t>& labels,
    std::vector<uint8_t>& out)
{
    if (t[0] == "lda") {
        out.push_back(LDA_IMM);
        out.push_back(parse_operand(t[1], labels));
    }
    else if (t[0] == "sta") {
        out.push_back(STA_MEM);
        out.push_back(parse_operand(t[1], labels));
    }
    else if (t[0] == "ina") {
        out.push_back(INA);
    }
    else if (t[0] == "cmp") {
        out.push_back(CMP_IMM);
        out.push_back(parse_operand(t[1], labels));
    }
    else if (t[0] == "jmp") {
        out.push_back(JMP);
        out.push_back(parse_operand(t[1], labels));
    }
    else if (t[0] == "beq") {
        out.push_back(BEQ);
        out.push_back(parse_operand(t[1], labels));
    }
    else if (t[0] == "hlt") {
        out.push_back(HLT);
    }
}



std::unordered_map<std::string,uint8_t> first_pass(const std::vector<std::string>& lines) {
    std::unordered_map<std::string,uint8_t> labels;
    uint8_t pc = 0;

    for (auto& line : lines) {
        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0].back() == ':') {
            std::string label = tokens[0];
            label.pop_back();
            labels[label] = pc;
            tokens.erase(tokens.begin());
            if (tokens.empty()) continue;
        }

        pc += instruction_size(tokens);
    }

    return labels;
}

std::vector<uint8_t> second_pass(const std::vector<std::string>& lines,
            const std::unordered_map<std::string,uint8_t>& labels) {

    std::vector<uint8_t> out;

    for (auto& line : lines) {
        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0].back() == ':') {
            tokens.erase(tokens.begin());
            if (tokens.empty()) continue;
        }

        assemble_instruction(tokens, labels, out);
    }

    return out;
}

std::vector<std::string> read_lines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}


std::vector<uint8_t> assemble(const std::string& filename) {
    auto lines = read_lines(filename);

    auto labels = first_pass(lines);
    auto output = second_pass(lines, labels);

    return output;
}

