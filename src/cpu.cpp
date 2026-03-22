#include "../include/cpu.h"
#include "../include/isa.h"
#include <iostream>

namespace {
constexpr uint8_t FLAG_Z = 1 << 0;
constexpr uint8_t FLAG_C = 1 << 1;
constexpr uint8_t FLAG_N = 1 << 2;

void update_zn(CPU& cpu, uint8_t value) {
    if (value == 0) cpu.FLAGS |= FLAG_Z;
    else cpu.FLAGS &= static_cast<uint8_t>(~FLAG_Z);

    if ((value & 0x80) != 0) cpu.FLAGS |= FLAG_N;
    else cpu.FLAGS &= static_cast<uint8_t>(~FLAG_N);
}

uint8_t read_mem(CPU& cpu, uint8_t addr) {
    if (addr < cpu.RAM.size()) {
        cpu.MR = cpu.RAM[addr];
        return cpu.MR;
    }
    cpu.MR = 0;
    return 0;
}

void write_mem(CPU& cpu, uint8_t addr, uint8_t value) {
    if (addr < cpu.RAM.size()) {
        cpu.RAM[addr] = value;
    }
    cpu.MR = value;
}

void alu_add(CPU& cpu, uint8_t rhs) {
    uint16_t sum = static_cast<uint16_t>(cpu.AC) + rhs;
    cpu.ALU = static_cast<uint8_t>(sum & 0xFF);
    if (sum > 0xFF) cpu.FLAGS |= FLAG_C;
    else cpu.FLAGS &= static_cast<uint8_t>(~FLAG_C);
    cpu.AC = cpu.ALU;
    update_zn(cpu, cpu.AC);
}

void alu_sub(CPU& cpu, uint8_t rhs) {
    uint16_t lhs = cpu.AC;
    uint16_t diff = static_cast<uint16_t>(lhs - rhs);
    cpu.ALU = static_cast<uint8_t>(diff & 0xFF);
    if (lhs >= rhs) cpu.FLAGS |= FLAG_C;
    else cpu.FLAGS &= static_cast<uint8_t>(~FLAG_C);
    cpu.AC = cpu.ALU;
    update_zn(cpu, cpu.AC);
}

void alu_cmp(CPU& cpu, uint8_t rhs) {
    uint16_t lhs = cpu.AC;
    uint16_t diff = static_cast<uint16_t>(lhs - rhs);
    cpu.ALU = static_cast<uint8_t>(diff & 0xFF);
    if (lhs >= rhs) cpu.FLAGS |= FLAG_C;
    else cpu.FLAGS &= static_cast<uint8_t>(~FLAG_C);
    update_zn(cpu, cpu.ALU);
}
}

void step(CPU& cpu) {
    if (cpu.halted || cpu.PC >= cpu.RAM.size())
        return;

    cpu.IR = cpu.RAM[cpu.PC];
    const IS* is = find_instruction_by_opcode(cpu.IR);
    int inst_size = is ? is->size : 1;

    if (inst_size >= 2 && cpu.PC + 1 < cpu.RAM.size())
        cpu.operand = cpu.RAM[cpu.PC + 1];
    else
        cpu.operand = 0;

    cpu.pc_overridden = false;
    controlLogic(cpu);

    if (!cpu.halted && !cpu.pc_overridden)
        cpu.PC = static_cast<uint8_t>(cpu.PC + inst_size);

    cpu.SC = static_cast<uint8_t>(cpu.SC + 1);
}

void controlLogic(CPU& cpu) {
    switch (cpu.IR) {
    case 0x00:
        // NOP
        break;
    case 0x01:
        // LDA_MEM: load from memory address (operand) into AC
        cpu.AC = read_mem(cpu, cpu.operand);
        update_zn(cpu, cpu.AC);
        break;
    case 0x02:
        // LDA_IMM: load immediate value (operand) into AC
        cpu.AC = cpu.operand;
        cpu.ALU = cpu.AC;
        update_zn(cpu, cpu.AC);
        break;
    case 0x03:
        // STA_MEM: store AC to memory address (operand)
        write_mem(cpu, cpu.operand, cpu.AC);
        break;
    case 0x04:
        // ADD_MEM: AC <- AC + MEM[operand]
        alu_add(cpu, read_mem(cpu, cpu.operand));
        break;
    case 0x05:
        // ADD_IMM: AC <- AC + operand
        alu_add(cpu, cpu.operand);
        break;
    case 0x06:
        // SUB_MEM: AC <- AC - MEM[operand]
        alu_sub(cpu, read_mem(cpu, cpu.operand));
        break;
    case 0x07:
        // SUB_IMM: AC <- AC - operand
        alu_sub(cpu, cpu.operand);
        break;
    case 0x08:
        // INC: AC <- AC + 1
        alu_add(cpu, 1);
        break;
    case 0x09:
        // DEC: AC <- AC - 1
        alu_sub(cpu, 1);
        break;
    case 0x0a:
        // CMP_MEM: set flags from AC - MEM[operand], AC unchanged
        alu_cmp(cpu, read_mem(cpu, cpu.operand));
        break;
    case 0x0b:
        // CMP_IMM: set flags from AC - operand, AC unchanged
        alu_cmp(cpu, cpu.operand);
        break;
    case 0x0c:
        // JMP: jump to address (operand)
        cpu.PC = cpu.operand;
        cpu.pc_overridden = true;
        break;
    case 0x0d:
        // BEQ: branch if equal (flags); operand = target address
        if (cpu.FLAGS & FLAG_Z) {
            cpu.PC = cpu.operand;
            cpu.pc_overridden = true;
        }
        break;
    case 0x0e:
        // BNE: branch if not equal (Z clear)
        if ((cpu.FLAGS & FLAG_Z) == 0) {
            cpu.PC = cpu.operand;
            cpu.pc_overridden = true;
        }
        break;
    case 0x0f:
        // OUT_MEM: print MEM[operand]
        std::cout << static_cast<int>(read_mem(cpu, cpu.operand)) << std::endl;
        break;
    case 0x10:
        // OUT_IMM: print operand immediate
        std::cout << static_cast<int>(cpu.operand) << std::endl;
        break;
    case 0x11:
        // OUT_ACC: print AC
        std::cout << static_cast<int>(cpu.AC) << std::endl;
        break;
    case 0xff:
        cpu.halted = true;
        break;
    default:
        std::cerr << "Unknown opcode 0x" << std::hex << static_cast<int>(cpu.IR) << std::dec << std::endl;
        cpu.halted = true;
        break;
    }
}
