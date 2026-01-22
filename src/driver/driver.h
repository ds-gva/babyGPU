#ifndef DRIVER_H
#define DRIVER_H

#include <stdio.h>
#include <stdint.h>

// opaque struct to shield the hardware
typedef struct GPU_Device GPU_Device;

GPU_Device* GPU_Device_create(const char* name);

// Initialization
GPU_Device* gpu_init(int width, int height);
void gpu_shutdown(GPU_Device* dev);

// State Configuration (The "API")
int gpu_load_program(GPU_Device* dev, const char* filename);
void gpu_set_constant(GPU_Device* dev, int index, uint32_t value);
void gpu_select_program(GPU_Device* dev, int program_id);

// Execution
int gpu_dispatch(GPU_Device* dev); 

// Getting the image (for the platform/monitor)
uint32_t* gpu_get_vram(GPU_Device* dev);


#endif