#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../isa/isa.h"
#include "gpu.h"




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
        if (inst.opcode > END) {
            gpu_trap(gpu, TRAP_OPCODE_UNKNOWN);
            printf("Opcode: %d\n", inst.opcode);
            running = 0;
            break;
        }

        // Check if the destination register is valid before running it
        if(inst.opcode != STORE_PIXEL && inst.opcode != END) {
            if (inst.dst >= NUM_REGS) {
                gpu_trap(gpu, TRAP_DEST_REGISTER_INVALID);
                running = 0;
                break;
            }
        }

        // Check if the source register is valid before running it
        if (inst.opcode != MOV_LOW_IMM8 && inst.opcode != MOV_HIGH_IMM8 && inst.opcode != STORE_PIXEL && inst.opcode != END) {
            if (inst.src0 >= NUM_REGS) {
                gpu_trap(gpu, TRAP_SRC_REGISTER_INVALID);
                running = 0;
                break;
            }
        }

        // Decode and execute the instruction
        switch(inst.opcode) {                
            case MOV_LOW_IMM8:
                // Broadcast the immediate value to all 32 threads
                for (int lane = 0; lane < 32; lane++) {
                    // Clear bottom 16 bits, then OR in the new value
                    gpu->registers[inst.dst][lane] &= 0xFF00; 
                    gpu->registers[inst.dst][lane] |= inst.src1_or_imm8;
                }
                break;

            case MOV_HIGH_IMM8: {
                for (int lane = 0; lane < 32; lane++) {
                    // Clear top 16 bits, then OR in the new value
                    gpu->registers[inst.dst][lane] &= 0x00FF; 
                    gpu->registers[inst.dst][lane] |= (inst.src1_or_imm8 << 8);
                }
                break;
            }

            case LDC: {
                uint8_t idx = inst.src1_or_imm8; // The index of the constant to load
                for (int lane = 0; lane < 32; lane++) {
                    gpu->registers[inst.dst][lane] = gpu->constants[idx];
                }
                break;
            }

            case LDI: {
                for (int lane = 0; lane < 32; lane++) {
                    gpu->registers[inst.dst][lane] = inst.src1_or_imm8;
                }
                break;
            }
            
            case MOV: {
                for (int lane = 0; lane < 32; lane++) {
                    gpu->registers[inst.dst][lane] = 
                        gpu->registers[inst.src0][lane];
                }
                break;
            }

            case ADD: {
                // Decode Operands
                for (int lane = 0; lane < 32; lane++) {
                    // dst = src0 + src1
                    gpu->registers[inst.dst][lane] = 
                        gpu->registers[inst.src0][lane] + gpu->registers[inst.src1_or_imm8][lane];
                }
                break;
            }

            case ADDI: {
                for (int lane = 0; lane < 32; lane++) {
                    gpu->registers[inst.dst][lane] += inst.src1_or_imm8; // Add the immediate value to the destination register
                }
                break;
            }

            case MULT: {
                for (int lane = 0; lane < 32; lane++) {
                    // dst = src0 * src1
                    gpu->registers[inst.dst][lane] = 
                        gpu->registers[inst.src0][lane] * gpu->registers[inst.src1_or_imm8][lane];
                }
                break;
            }

            case MULTI: {
                for (int lane = 0; lane < 32; lane++) {
                    // dst = src0 * imm8
                    gpu->registers[inst.dst][lane] = 
                        gpu->registers[inst.src0][lane] * inst.src1_or_imm8;
                }
                break;
            }

            case DIV: {
                for (int lane = 0; lane < 32; lane++) {
                    // dst = src0 / src1
                    if (gpu->registers[inst.src1_or_imm8][lane] != 0) {
                        gpu->registers[inst.dst][lane] = 
                            gpu->registers[inst.src0][lane] / gpu->registers[inst.src1_or_imm8][lane];
                    } else {
                        gpu_trap(gpu, TRAP_DIVIDE_BY_ZERO);
                        running = 0;
                        break;
                    }
                }
                break;
            }

            case SLT: {
                for (int lane = 0; lane < 32; lane++) {
                    // dst = (src0 < src1) ? 1 : 0
                    if (gpu->registers[inst.src0][lane] < gpu->registers[inst.src1_or_imm8][lane]) {
                        gpu->registers[inst.dst][lane] = 1;
                    } else {
                        gpu->registers[inst.dst][lane] = 0;
                    }
                }
                break;
            }

            case STORE_PIXEL: {
                for (int lane = 0; lane < 32; lane++) {

                    // The hardware knows where the thread belongs!
                    int address = start_pixel + lane;
                    
                    if (address < VRAM_SIZE) {
                        gpu->vram[address] = gpu->registers[0][lane];
                    }
                }
                break;
            }

            case END:
                running = 0; 
                break;
        }
        
        gpu->pc++;
    }
}

