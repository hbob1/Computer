#pragma once
#include <cstdint>

enum Opcode : uint8_t {
    NOP = 0x00,

    LDA_IMM = 0x01,
    LDA_MEM = 0x02,
    STA_MEM = 0x03,

    INA     = 0x04,

    CMP_IMM = 0x05,

    JMP     = 0x06,
    BEQ     = 0x07,

    HLT     = 0xFF
};