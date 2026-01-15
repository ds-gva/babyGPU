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

    uint32_t r0;             // A generic register
    int pc;                  // Program Counter
};


void gpu_execute_warp(struct babyGPU *gpu, int start_pixel, int end_pixel) {
    gpu->pc = 0;
    int running = 1;

    while(running) {

        // FETCH
        struct Instruction inst = gpu->code_memory[gpu->pc];
        
        // DECODE & EXECUTE
        switch(inst.opcode) {
            case OP_END:
                running = 0; 
                break;
                
            case OP_MOV_LOW:
                gpu->r0 = inst.value; 
                break;

            case OP_MOV_HIGH:
                gpu->r0 &= 0x0000FFFF; 
                gpu->r0 |= (inst.value << 16);
                break;

            case OP_STORE_PIXEL:
                for (int i = start_pixel; i < end_pixel; i++) {
                    gpu->vram[i] = gpu->r0;
                }
                break;
        }
        
        gpu->pc++;
    }
}

int main() {

    struct babyGPU gpu;
    gpu.vram = malloc(MEM_SIZE * sizeof(uint32_t));
    gpu.code_memory = malloc(PROG_SIZE * sizeof(struct Instruction));

    if (!platform_init(WIDTH, HEIGHT, "Realistic Tiny GPU")) return -1;

    
    // "Compile" our Shader (CPU creates instructions)
    // Color is R, G, B, A
    // LOW: 0xGGRR
    // HIGH: 0xFFBB

    struct Instruction shader[] = {
        { OP_MOV_LOW,  0, 0xD2FA }, 
        { OP_MOV_HIGH, 0, 0xFFB4 },
        { OP_STORE_PIXEL, 0, 0 }, 
        { OP_END, 0, 0 } 
    };

    // "Upload" code to GPU (The Bus Transfer)
    // In reality, this is a PCIe copy
    for (int i=0; i<3; i++) gpu.code_memory[i] = shader[i];

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
    return 0;
}