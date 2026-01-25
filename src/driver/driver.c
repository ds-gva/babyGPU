#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "driver.h"
#include "../gpu/gpu.h"
#include "shader_assembler.h"

#define MAX_PROGRAMS 16

typedef struct {
    struct Instruction* code;
    int size;
    bool active;
} LoadedProgram;

struct GPU_Device {
    // The underlying "hardware" state
    struct babyGPU hardware; 

    LoadedProgram programs[MAX_PROGRAMS];
    int program_count;

    // Driver-specific state (meta-data)
    int screen_width;
    int screen_height;
    int active_program_id;
    int active_program_size;

    bool is_initialized;
};



GPU_Device* gpu_init(int width, int height) {

    // Allocate the wrapper struct
    GPU_Device* dev = malloc(sizeof(GPU_Device));
    if (!dev) return NULL;

    // Initialize "driver" state
    dev->screen_width = width;
    dev->screen_height = height;
    dev->active_program_id = 0;

    // Initialize "Hardware" State and allocating vram
    dev->hardware.vram = malloc(width * height * sizeof(uint32_t));
    dev->hardware.code_memory = malloc(PROG_SIZE * sizeof(struct Instruction));
    dev->hardware.pc = 0;
    dev->hardware.tripped = false;
    
    // Safety check
    if (!dev->hardware.vram) {
        free(dev);
        return NULL;
    }

    return dev;
}

void gpu_shutdown(GPU_Device* dev) {
    if (!dev) return;
    
    // Clean up hardware resources
    if (dev->hardware.vram) free(dev->hardware.vram);
    if (dev->hardware.code_memory) free(dev->hardware.code_memory);

    // Free the device handle itself
    free(dev);
}

int gpu_load_program(GPU_Device* dev, const char* filename) {
    if (dev->program_count >= MAX_PROGRAMS) {
        printf("Driver Error: Max programs reached\n");
        return -1;
    }

    
    // Read the shader file (original parsing)
    InstructionTextList list = read_shader_text(filename); // need to add a way to fail gracefully
    int prog_size = 0;
    struct Instruction *code = assemble_shader(list, &prog_size);
    free(list.data);

    if (code == NULL) {
        printf("Shader compilation failed\n");
        return -1;
    }

    int id = dev->program_count;
    dev->programs[id].code = code;
    dev->programs[id].size = prog_size;
    dev->programs[id].active = true;

    printf("Program loaded successfully with ID: %d\n", id);

    dev->program_count++;

    return id;
}

void gpu_select_program(GPU_Device* dev, int program_id) {
    if (program_id < 0 || program_id >= dev->program_count) return;
    if (!dev->programs[program_id].active) return;

    LoadedProgram* prog = &dev->programs[program_id];

    //simulating clearing the code memory
    memset(dev->hardware.code_memory, 0, PROG_SIZE * sizeof(struct Instruction));

    int copy_size = (prog->size > PROG_SIZE) ? PROG_SIZE : prog->size;

    // simulating copying the code to the code memory
    memcpy(dev->hardware.code_memory, prog->code, copy_size * sizeof(struct Instruction));

    dev->active_program_id = program_id;
    dev->active_program_size = copy_size;
}

void gpu_set_constant(GPU_Device* dev, int index, uint32_t value) {
    if (index < 0 || index >= 256) return;
    dev->hardware.constants[index] = value;
}

int gpu_dispatch(GPU_Device* dev) {
    if (dev->active_program_id == -1) {
        printf("Driver Warning: No program selected before dispatch.\n");
        return -1;
    }

    int warp_size = 32;
    int prog_size = dev->active_program_size;

    dev->hardware.tripped = false;

    // Simulating the "Command Processor" distributing threads
    for (int i = 0; i < VRAM_SIZE; i += warp_size) {
        gpu_execute_warp(&dev->hardware, i, i + warp_size, prog_size);
        
        // Check for hardware faults (traps)
        if (dev->hardware.tripped) {
            return 1;
        }
    }

    return 0;
}

uint32_t* gpu_get_vram(GPU_Device* dev) {
    return dev->hardware.vram;
}