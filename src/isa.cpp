#include "../include/isa.h"
#include <algorithm>
#include <cctype>

IS NOP     = {1, 0x00, "NOP"};

IS LDA_MEM = {2, 0x01, "LDA MEM"};
IS LDA_IMM = {2, 0x02, "LDA IMM"};
IS STA_MEM = {2, 0x03, "STA MEM"};
IS ADD_MEM = {2, 0x04, "ADD MEM"};
IS ADD_IMM = {2, 0x05, "ADD IMM"};
IS SUB_MEM = {2, 0x06, "SUB MEM"};
IS SUB_IMM = {2, 0x07, "SUB IMM"};
IS INC     = {1, 0x08, "INC"};
IS DEC     = {1, 0x09, "DEC"};
IS CMP_MEM = {2, 0x0a, "CMP MEM"};
IS CMP_IMM = {2, 0x0b, "CMP IMM"};
IS JMP     = {2, 0x0c, "JMP"};
IS BEQ     = {2, 0x0d, "BEQ"};
IS BNE     = {2, 0x0e, "BNE"};
IS OUT_MEM = {2, 0x0f, "OUT MEM"};
IS OUT_IMM = {2, 0x10, "OUT IMM"};
IS OUT_ACC = {1, 0x11, "OUT"};

IS HLT     = {1, 0xff, "HLT"};

static std::string first_word_lower(const std::string& name) {
    std::string first;
    for (char c : name) {
        if (c == ' ') break;
        first += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return first;
}

static const IS* const instructions[] = {
    &NOP,
    &LDA_MEM, &LDA_IMM,
    &STA_MEM,
    &ADD_MEM, &ADD_IMM,
    &SUB_MEM, &SUB_IMM,
    &INC, &DEC,
    &CMP_MEM, &CMP_IMM,
    &JMP, &BEQ, &BNE,
    &OUT_MEM, &OUT_IMM, &OUT_ACC,
    &HLT
};

static bool name_has_mem(const std::string& name) {
    return name.find("MEM") != std::string::npos;
}

const IS* find_instruction(
    const std::string& mnemonic,
    bool operand_is_memory,
    bool has_operand)
{
    std::string m = mnemonic;
    for (char& c : m) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    const IS* fallback = nullptr;
    for (const IS* is : instructions) {
        if (first_word_lower(is->name) != m) continue;
        fallback = is;
        if (!has_operand) {
            if (is->size == 1) return is;
            continue;
        }
        if (name_has_mem(is->name) == operand_is_memory)
            return is;
    }
    return fallback;
}

const IS* find_instruction_by_opcode(uint8_t opcode) {
    for (const IS* is : instructions) {
        if (is->address == opcode)
            return is;
    }
    return nullptr;
}
