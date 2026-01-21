#ifndef SHADER_ASSEMBLER_H
#define SHADER_ASSEMBLER_H
#include <stdint.h>
#include "../isa/isa.h"

typedef struct {
    char tok[4][64];
    int count;
    int line_num;
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

// Assemble a list of InstructionText into a list of Instructions ready to be uploaded to theGPU
struct Instruction* assemble_shader(InstructionTextList list, int *prog_size);

#endif