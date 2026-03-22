#pragma once
#include <cstdint>
#include <string>

struct IS {
    int size;
    uint8_t address;
    std::string name;
};

const IS* find_instruction(
    const std::string& mnemonic,
    bool operand_is_memory = false,
    bool has_operand = true);

// Look up instruction by opcode byte (e.g. 0x01 for LDA_MEM). Returns nullptr if unknown.
const IS* find_instruction_by_opcode(uint8_t opcode);

/*
enum Opcode : uint8_t {
    NOP = 0x00,

    LDA_IMM = 0x01,
    LDA_MEM = 0x02,
    STA_MEM = 0x03,

    INA     = 0x04,

    CMP_IMM = 0x05,

    JMP     = 0x06,
    BEQ     = 0x07,

    INC     = 0x08,

    HLT     = 0xFF
};
*/
