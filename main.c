#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "platform.h"
#include "isa.h"

#define WIDTH 640
#define HEIGHT 480
#define MEM_SIZE (WIDTH * HEIGHT) // Total VRAM size (pixels)
#define PROG_SIZE 1024            // Size reserved for code

// Simulated GPU
struct babyGPU {
    uint32_t *vram;
    struct Instruction *code_memory;

    uint32_t r0[32];             // Register to hold color
    uint32_t r1[32];            // x coordinates
    uint32_t r2[32];            // y coordinates

    int pc;                  // Program Counter
};


void gpu_execute_warp(struct babyGPU *gpu, int start_pixel, int end_pixel) {
    gpu->pc = 0;
    int running = 1;

    for (int lane = 0; lane < 32; lane++) {
        int global_index = start_pixel + lane;

        int x = global_index % WIDTH;
        int scaled_x = (x * 255) / WIDTH;
        gpu->r1[lane] = scaled_x; // we spread out our color along the pixels

        gpu->r2[lane] = global_index / WIDTH;
    
        gpu->r0[lane] = 0;
    }

    while(running) {

        // FETCH
        struct Instruction inst = gpu->code_memory[gpu->pc];
        
        // DECODE & EXECUTE
        switch(inst.opcode) {
            case OP_END:
                running = 0; 
                break;
                
            case OP_MOV_LOW:
                // Broadcast the immediate value to all 32 threads
                for (int lane = 0; lane < 32; lane++) {
                    // Clear bottom 16 bits, then OR in the new value
                    gpu->r0[lane] &= 0xFFFF0000; 
                    gpu->r0[lane] |= inst.value;
                }
                break;

            case OP_MOV_HIGH:
                for (int lane = 0; lane < 32; lane++) {
                    // Clear top 16 bits, then OR in the new value
                    gpu->r0[lane] &= 0x0000FFFF; 
                    gpu->r0[lane] |= (inst.value << 16);
                }
                break;

            case OP_STORE_PIXEL:
                for (int lane = 0; lane < 32; lane++) {
                    // The hardware knows where the thread belongs!
                    int address = start_pixel + lane;
                    
                    if (address < MEM_SIZE) {
                        gpu->vram[address] = gpu->r0[lane];
                    }
                }
                break;
            
            case OP_ADD:
                for (int lane = 0; lane < 32; lane++) {
                    gpu->r0[lane] += gpu->r1[lane];
                }
        }
        
        gpu->pc++;
    }
}

int main() {

    struct babyGPU gpu;
    gpu.vram = malloc(MEM_SIZE * sizeof(uint32_t));
    gpu.code_memory = malloc(PROG_SIZE * sizeof(struct Instruction));

    if (!platform_init(WIDTH, HEIGHT, "BabyGPU")) return -1;

    
    // "Compile" our Shader (CPU creates instructions)
    // Color is R, G, B, A
    // LOW: 0xGGRR
    // HIGH: 0xFFBB
    // We add r0 to r1 ; very basic way of creating a gradient
    struct Instruction shader[] = {
        { OP_MOV_LOW,  0, 0x0000 }, 
        { OP_MOV_HIGH, 0, 0xFF00 },
        { OP_ADD, 0, 0 },
        { OP_STORE_PIXEL, 0, 0 }, 
        { OP_END, 0, 0 } 
    };

    int prog_size = sizeof(shader) / sizeof(shader[0]);

    // "Upload" code to GPU (The Bus Transfer)
    // In reality, this is a PCIe copy
    for (int i=0; i<prog_size; i++) gpu.code_memory[i] = shader[i];

    while(1) {

        // Send to GPU
        int warp_size = 32;
        for (int i = 0; i < MEM_SIZE; i += warp_size) {
            gpu_execute_warp(&gpu, i, i + warp_size);
        }

        // Send to "Monitor"
        if (!platform_update(gpu.vram)) break;
    }

    platform_terminate();
    free(gpu.vram);
    free(gpu.code_memory);
    return 0;
}