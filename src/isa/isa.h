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

    // Load operations
    MOV_LOW_IMM8 = 0,   // Load value into bottom 8 bits (preserves top) {mov_low_imm8, dst, imm8}
    MOV_HIGH_IMM8 = 1,  // Load value into top 8 bits (preserves bottom) {mov_high_imm8, dst, imm8}
    LDC = 2, // Load constant {ldc, dst, imm8}
    LDI = 3, // Load immediate value {ldi, dst, imm8}

    // Move operations
    MOV = 4, // Move value from one register to another {mov, dst, src0}

    // Arithmetic operations
    ADD = 5, // Addition of two registers {add, dst, src0, src1}
    ADDI = 6, // Addition of a register and an immediate value {addi, dst, src0, imm8}
    MULT = 7, // Multiplication of two registers {mult, dst, src0, src1}
    MULTI = 8, // Multiplication of a register and an immediate value {multi, dst, src0, imm8}
    DIV = 9, // Division of two registers {div, dst, src0, src1}
    DIVI = 10, // Division of a register and an immediate value {divi, dst, src0, imm8}

    // Logic operations
    SLT = 11, // Set less than of two registers {slt, dst, src0, src1}

    // Branching operaitons
    JMP = 12, // Unconditional jump {JMP, target} - to change: this will mean max of 256 instructions

    // Store operations
    STORE_PIXEL = 13, // Store the value in the R0 register to the VRAM {store_pixel}

    // End operation
    END =  14 // End instruction {end}
};


// A single instruction
// opcode: The operation to perform
// reg_dest: The destination register
// src0: The first source register
// src1_or_imm8: The second source register or immediate value
struct Instruction {
    uint8_t opcode;
    uint8_t dst;
    uint8_t src0;
    uint8_t src1_or_imm8;
};

#endif