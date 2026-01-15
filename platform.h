#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

// Call this once to open the window
int platform_init(int width, int height, const char* title);

// Call this every frame to send your VRAM to the window
// Returns 0 if the window should close, 1 otherwise
int platform_update(uint32_t* buffer);

// Call this when you are done
void platform_terminate();

#endif