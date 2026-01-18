#ifndef ISA_H
#define ISA_H
#include <stdint.h>

// Our register aliases, to make our "assembly" more undersandable
enum RegAlias {
    R0_COLOR_OUT = 0, // output our colors
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7,
    R8 = 8
};

// The Opcodes
enum OpCode {
    OP_MOV_LOW_IMM16 = 0,   // Load value into bottom 16 bits (preserves top)
    OP_MOV_HIGH_IMM16 = 1,  // Load value into top 16 bits (preserves bottom)
    OP_ADD_REG = 2, // Addition of two registers
    OP_MULT_REG = 3, // Multiplication of two registers
    OP_DIV_REG = 4, // Division of two registers
    OP_SLT_REG = 5, // Set less than of two registers
    OP_STORE_PIXEL = 6, // Store the value in the R0 register to the VRAM
    OP_END =  7 // End instruction
};


// A single instruction
// opcode: The operation to perform
// reg_dest: The destination register
// value: Either a imm16 (immediate value) or a regster
struct Instruction {
    uint8_t opcode; 
    uint8_t reg_dest;
    uint16_t value; // If opcodes terminate with IMM16 then value is the immediate value, if they terminate with REG then value is the source register
};

#endif