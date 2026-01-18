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


// Load a .shader file and parse each line into an InstructionText
// Returns a list of InstructionText
InstructionTextList read_shader_text(const char *filename);

// Compile a list of InstructionText into a list of Instructions ready to be uploaded to theGPU
struct Instruction* compile_shader(InstructionTextList list, int *prog_size);

#endif