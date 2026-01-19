#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "platform/platform.h"
#include "isa/isa.h"

#include "gpu/gpu.h"
#include "driver/shader_assembler.h"

int main() {

    // Initialize our fake monitor (the platform, GLFW)
    if (!platform_init(WIDTH, HEIGHT, "BabyGPU")){
        printf("Failed to initialize platform\n");
        return -1;
    } 

    int prog_size;
    // Read the shader file (original parsing)
    InstructionTextList list = read_shader_text("test_shader.shader"); // need to add a way to fail gracefully
    // Assemble the shader (basically store instructions in numeric format to be passed on to the GPU)
    struct Instruction *shader = assemble_shader(list, &prog_size);

    
    if (shader == NULL) {
        printf("Shader compilation failed\n");
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