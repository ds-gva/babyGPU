#ifndef SHADER_COMPILER_H
#define SHADER_COMPILER_H
#include <stdint.h>
#include "isa.h"

typedef struct {
    char opcode[64];
    char reg_dest[64];
    char value[64];
} InstructionText;

typedef struct {
    InstructionText *data;
    size_t len;
} InstructionTextList;

typedef struct { 
    const char *name;
    uint8_t opcode; 
} OpcodeEntry;

typedef struct  {
    const char *name;
    uint8_t reg;
} RegEntry;


// Use this to convert my opcodes to the enums
static const OpcodeEntry kOpcodes[] = {
    {"OP_MOV_LOW_IMM16",  OP_MOV_LOW_IMM16},
    {"OP_MOV_HIGH_IMM16", OP_MOV_HIGH_IMM16},
    {"OP_ADD_REG",        OP_ADD_REG},
    {"OP_MULT_REG",       OP_MULT_REG},
    {"OP_DIV_REG",        OP_DIV_REG},
    {"OP_SLT_REG",        OP_SLT_REG},
    {"OP_STORE_PIXEL",    OP_STORE_PIXEL},
    {"OP_END",            OP_END},
};
// Use this to convert my registers to the enums
static const RegEntry kRegs[] = {
    {"R0_COLOR_OUT", R0_COLOR_OUT},
    {"R1",           R1},
    {"R2",           R2},
    {"R3",           R3},
    {"R4",           R4},
    {"R5",           R5},
    {"R6",           R6},
    {"R7",           R7},
    {"R8",           R8},
};

// Load a .shader file and parse each line into an InstructionText
// Returns a list of InstructionText
InstructionTextList read_shader_text(const char *filename);

// Compile a list of InstructionText into a list of Instructions ready to be uploaded to theGPU
struct Instruction* compile_shader(InstructionTextList list, int *prog_size);

#endif