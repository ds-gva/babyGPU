#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "platform.h"
#include "isa.h"
#include "shader_compiler.h"

#define WIDTH 640
#define HEIGHT 480
#define VRAM_SIZE (WIDTH * HEIGHT) // Total VRAM size (pixels)
#define PROG_SIZE 1024            // Instruction slots reserved for code

#define NUM_REGS 9

// Simulated GPU
struct babyGPU {
    uint32_t *vram;
    struct Instruction *code_memory;

    uint32_t registers[NUM_REGS][32];
    int pc;                  // Program Counter
    bool tripped;
};

enum TrapReason {
    TRAP_DIVIDE_BY_ZERO = 0,
    TRAP_PC_OUT_OF_BOUNDS = 1,
    TRAP_OPCODE_UNKNOWN = 2,
    TRAP_SRC_REGISTER_INVALID = 3,
    TRAP_DEST_REGISTER_INVALID = 4,
    TRAP_INSTRUCTION_MEMORY_OVERFLOW = 5
};

void gpu_trap(struct babyGPU *gpu, enum TrapReason reason) {
    if(gpu->tripped) return;
    printf("GPU Trap at PC #: %d\n", gpu->pc);
    printf("Trap Reason: %d\n", reason);
    gpu->tripped = true;
    return;
}

void gpu_execute_warp(struct babyGPU *gpu, int start_pixel, int end_pixel, int prog_size) {
    gpu->pc = 0;
    int running = 1;
    // INIT REGISTERS
    for (int lane = 0; lane < 32; lane++) {
        for (int r = 0; r < NUM_REGS; r++) {
            gpu->registers[r][lane] = 0;
        }
        int global_index = start_pixel + lane;
        
        // Calculate coords
        int x = global_index % WIDTH;
        int y = global_index / WIDTH;
        
        // R0: Reserved for Output Color.
        // R1: Initialized with X Coordinate (UV).
        // R2: Initialized with Y Coordinate.
        // Rest: Empty (available for the shader to use!).
        // R0: Reset Accumulator/Color
        gpu->registers[0][lane] = 0;

        // R1: Load Scaled X (0-255)
        gpu->registers[1][lane] = x;

        // R2: Load Scaled Y (0-255)
        gpu->registers[2][lane] = y; 
    }

    while(running) {

        // Is PC out of bounds?
        if (gpu->pc < 0 || gpu->pc >= prog_size) {
            gpu_trap(gpu, TRAP_PC_OUT_OF_BOUNDS);
            printf("PC: %d, Prog Size: %d\n", gpu->pc, prog_size);
            running = 0;
            break;
        }

        // Get instructions from the GPU's code memory
        struct Instruction inst = gpu->code_memory[gpu->pc];

        // Check if opcode is valid before running it
        if (inst.opcode > OP_END) {
            gpu_trap(gpu, TRAP_OPCODE_UNKNOWN);
            printf("Opcode: %d\n", inst.opcode);
            running = 0;
            break;
        }

        // Check if the destination register is valid before running it
        if(inst.opcode != OP_STORE_PIXEL && inst.opcode != OP_END) {
            if (inst.reg_dest >= NUM_REGS) {
                gpu_trap(gpu, TRAP_DEST_REGISTER_INVALID);
                running = 0;
                break;
            }
        }

        // Check if the source register is valid before running it
        if (inst.opcode != OP_MOV_LOW_IMM16 && inst.opcode != OP_MOV_HIGH_IMM16 && inst.opcode != OP_STORE_PIXEL && inst.opcode != OP_END) {
            if (inst.value >= NUM_REGS) {
                gpu_trap(gpu, TRAP_SRC_REGISTER_INVALID);
                running = 0;
                break;
            }
        }

        // Decode and execute the instruction
        switch(inst.opcode) {                
            case OP_MOV_LOW_IMM16:
                // Broadcast the immediate value to all 32 threads
                for (int lane = 0; lane < 32; lane++) {
                    // Clear bottom 16 bits, then OR in the new value
                    gpu->registers[inst.reg_dest][lane] &= 0xFFFF0000; 
                    gpu->registers[inst.reg_dest][lane] |= inst.value;
                }
                break;

            case OP_MOV_HIGH_IMM16: {
                for (int lane = 0; lane < 32; lane++) {
                    // Clear top 16 bits, then OR in the new value
                    gpu->registers[inst.reg_dest][lane] &= 0x0000FFFF; 
                    gpu->registers[inst.reg_dest][lane] |= (inst.value << 16);
                }
                break;
            }
            
            case OP_ADD_REG: {
                // Decode Operands
                uint8_t dest_reg = inst.reg_dest;   // Destination  (added to the below)
                uint16_t src_reg = inst.value;      // Which register to add
                
                for (int lane = 0; lane < 32; lane++) {
                    // GENERIC ADD: Reg[Dest] += Reg[Src]
                    gpu->registers[dest_reg][lane] += gpu->registers[src_reg][lane];
                }
                break;
            }

            case OP_MULT_REG: {
                uint8_t dest_reg = inst.reg_dest;   //  Destination multiplied by the below
                uint16_t src_reg = inst.value;      // Which register to multiply with

                for (int lane = 0; lane < 32; lane++) {
                    // GENERIC MULT: Reg[Dest] *= Reg[Src]
                    gpu->registers[dest_reg][lane] *= gpu->registers[src_reg][lane];
                }
                break;
            }

            case OP_DIV_REG: {
                uint8_t dest_reg = inst.reg_dest;   //  Destination divided by the below
                uint16_t src_reg = inst.value;      // Which register to divide by

                for (int lane = 0; lane < 32; lane++) {
                    uint32_t divisor = gpu->registers[src_reg][lane];
                    
                    // CRITICAL SAFETY CHECK
                    if (divisor != 0) {
                        gpu->registers[dest_reg][lane] /= divisor;
                    } else {
                        // Handle divide by zero (usually keep value or set to MAX)
                        gpu->registers[dest_reg][lane] = 0xFFFFFFFF; 
                    }
                }
                break;
            }

            case OP_SLT_REG: { // set less than
                uint8_t dest_reg = inst.reg_dest;   //  Destination set less than the below
                uint16_t src_reg = inst.value;      // Which register to set less than

                for (int lane = 0; lane < 32; lane++) {
                    if  (gpu->registers[dest_reg][lane] <  gpu->registers[src_reg][lane]){
                        gpu->registers[dest_reg][lane] = 1; // True
                    } else {
                        gpu->registers[dest_reg][lane] = 0; // False
                    }
                }
                break;
            }

            case OP_STORE_PIXEL: {
                for (int lane = 0; lane < 32; lane++) {

                    // The hardware knows where the thread belongs!
                    int address = start_pixel + lane;
                    
                    if (address < VRAM_SIZE) {
                        gpu->vram[address] = gpu->registers[0][lane];
                    }
                }
                break;
            }

            case OP_END:
                running = 0; 
                break;
        }
        
        gpu->pc++;
    }
}


int main() {

    // Initialize our fake monitor (the platform, GLFW)
    if (!platform_init(WIDTH, HEIGHT, "BabyGPU")){
        printf("Failed to initialize platform\n");
        return -1;
    } 

    int prog_size;
    // Read the shader file (original parsing)
    InstructionTextList list = read_shader_text("test_shader.shader"); // need to add a way to fail gracefully
    // Compile the shader (basically store instructions in numeric format to be passed on to the GPU)
    struct Instruction *shader = compile_shader(list, &prog_size);

    free(list.data);
    if (shader == NULL) {
        printf("Shader compilation failed\n");
        free(shader);
        platform_terminate();
        return -1;
    }

    // Initialize our GPU
    struct babyGPU gpu;
    gpu.tripped = false;
    gpu.vram = malloc(VRAM_SIZE * sizeof(uint32_t));
    gpu.code_memory = malloc(PROG_SIZE * sizeof(struct Instruction));

    // Check if the shader is too big for the GPU
    if (prog_size > PROG_SIZE) {
        gpu_trap(&gpu, TRAP_INSTRUCTION_MEMORY_OVERFLOW);
        free(list.data);
        free(shader);
        free(gpu.vram);
        free(gpu.code_memory);
        platform_terminate();
        return -1;
    }

    // "Upload" code to GPU (The Bus Transfer)
    for (int i=0; i < prog_size; i++) gpu.code_memory[i] = shader[i];

    while(gpu.tripped == false) {

        int warp_size = 32;
        // Execute the shader on the GPU (we probably want to move this is a GPU-specific function at some point)
        for (int i = 0; i < VRAM_SIZE; i += warp_size) {
            gpu_execute_warp(&gpu, i, i + warp_size, prog_size);
            if(gpu.tripped) break;
        }
        // Send to "Monitor"
        if (!platform_update(gpu.vram)) break;
    }

    platform_terminate();
    free(gpu.vram);
    free(gpu.code_memory);
    return 0;
}