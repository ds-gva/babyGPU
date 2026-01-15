#ifndef ISA_H
#define ISA_H
#include <stdint.h>

// The Opcodes (What can the GPU do?)
enum OpCode {
    OP_END = 0,
    OP_MOV_LOW = 1,   // Load value into bottom 16 bits (clears top)
    OP_MOV_HIGH = 2,  // Load value into top 16 bits (preserves bottom)
    OP_STORE_PIXEL = 3
};

struct Instruction {
    uint8_t opcode;
    uint8_t reg_dest;
    uint16_t value;
};

#endif