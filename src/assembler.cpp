#include "../include/assembler.h"
#include "../include/isa.h"
#include <cstdint>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cctype>

std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;
    while (stream >> token) {
        for (char& c : token) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        tokens.push_back(token);
    }
    return tokens;
}

static std::string strip_comment(std::string line) {
    size_t sc = line.find(';');
    if (sc != std::string::npos) {
        line = line.substr(0, sc);
    }
    return line;
}

static bool operand_is_memory(const std::vector<std::string>& tokens) {
    return tokens.size() >= 2 && !tokens[1].empty() && tokens[1][0] == '$';
}

int instruction_size(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return 0;
    bool has_operand = tokens.size() >= 2;
    const IS* is = find_instruction(tokens[0], operand_is_memory(tokens), has_operand);
    return is ? is->size : 0;
}

static uint8_t parse_operand(
    std::string s,
    const std::unordered_map<std::string,uint8_t>& labels)
{
    if (s.empty()) return 0;
    if (s[0] == '$')
        s = s.substr(1);
    if (s.empty()) return 0;
    if (std::isdigit(static_cast<unsigned char>(s[0])) || s[0] == '-' ||
        (s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')))
        return static_cast<uint8_t>(std::stoi(s, nullptr, 0));
    return labels.at(s);
}

static void assemble_instruction(
    const std::vector<std::string>& t,
    const std::unordered_map<std::string,uint8_t>& labels,
    std::vector<uint8_t>& out)
{
    bool mem = operand_is_memory(t);
    bool has_operand = t.size() >= 2;
    const IS* is = find_instruction(t[0], mem, has_operand);
    if (!is) return;

    out.push_back(is->address);
    if (is->size >= 2 && has_operand)
        out.push_back(parse_operand(t[1], labels));
}



std::unordered_map<std::string,uint8_t> first_pass(const std::vector<std::string>& lines) {
    std::unordered_map<std::string,uint8_t> labels;
    uint8_t pc = 0;

    for (const auto& raw_line : lines) {
        std::string line = strip_comment(raw_line);
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

    for (const auto& raw_line : lines) {
        std::string line = strip_comment(raw_line);
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

