#ifndef GPU_H
#define GPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../isa/isa.h"

#define WIDTH 640
#define HEIGHT 480
#define VRAM_SIZE (WIDTH * HEIGHT) // Total VRAM size (pixels)
#define PROG_SIZE 1024            // Instruction slots reserved for code

#define NUM_REGS 9

// Simulated GPU
struct babyGPU {
    uint32_t *vram; // The VRAM
    struct Instruction *code_memory; // The shader code
    uint32_t constants[256]; // The constant buffer (uniforms)

    uint32_t registers[NUM_REGS][32]; // 9 registers, 32 lanes
    int pc;   // Program Counter
    bool tripped; // Whether the GPU has tripped
};

enum TrapReason {
    TRAP_DIVIDE_BY_ZERO = 0,
    TRAP_PC_OUT_OF_BOUNDS = 1,
    TRAP_OPCODE_UNKNOWN = 2,
    TRAP_SRC_REGISTER_INVALID = 3,
    TRAP_DEST_REGISTER_INVALID = 4,
    TRAP_INSTRUCTION_MEMORY_OVERFLOW = 5,
    TRAP_CONSTANT_BUFFER_OVERFLOW = 6
};

void gpu_execute_warp(struct babyGPU *gpu, int start_pixel, int end_pixel, int prog_size);

#endif