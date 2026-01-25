#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "platform/platform.h"
#include "driver/driver.h"

#include "gpu/gpu.h"

int main() {

    printf("Starting BabyGPU\n");
    // Initialize our fake monitor (the platform, GLFW)
    if (!platform_init(WIDTH, HEIGHT, "BabyGPU")){
        printf("Failed to initialize platform\n");
        return -1;
    } 

    printf("Platform initialized\n");

    GPU_Device* dev = gpu_init(WIDTH, HEIGHT);
    if (dev == NULL) {
        printf("Failed to initialize GPU\n");
        platform_terminate();
        return -1;
    }

    printf("GPU initialized\n");
    int prog_id = gpu_load_program(dev, "checker.shader");
    printf("Program loaded\n");
    
    gpu_select_program(dev, prog_id);
    gpu_set_constant(dev, 0, 0xFF0000FF);  
    gpu_set_constant(dev, 1, 0xFF00FF00);
/*     gpu_set_constant(dev, 0, 0xFF0000FF); // square color
    gpu_set_constant(dev, 1, 270); // square x
    gpu_set_constant(dev, 2, 370); // square x
    gpu_set_constant(dev, 3, 190); // square y
    gpu_set_constant(dev, 4, 290); // square y */

    bool running = true;

    while(running) {

        int status = gpu_dispatch(dev);
        
        if (status != 0) {
            printf("Main Loop: GPU has tripped. Exiting.\n");
            running = false; 
            break;
        }

        if (!platform_update(gpu_get_vram(dev))) {
            running = false;
        }
    }

    platform_terminate();
    return 0;

}