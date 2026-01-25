#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

#include "platform/platform.h"
#include "driver/driver.h"

#include "gpu/gpu.h"

#define CLEAR_SCREEN "\033[2J\033[H"
#define BOLD         "\033[1m"
#define RESET        "\033[0m"
#define GREEN        "\033[32m"
#define RED          "\033[31m"

void enable_ansi_support() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void print_header() {
    printf("\n");
    printf("  \033[38;5;39m╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                                                                                      \033[38;5;39m  ║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                          \033[38;5;207m _           _            \033[38;5;189m ██████╗ \033[38;5;153m██████╗ \033[38;5;117m██╗   ██╗\033[0m                          \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                          \033[38;5;207m| |__   __ _| |__  _   _  \033[38;5;189m██╔════╝ \033[38;5;153m██╔══██╗\033[38;5;117m██║   ██║\033[0m                          \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                          \033[38;5;211m| '_ \\ / _` | '_ \\| | | | \033[38;5;189m██║  ███╗\033[38;5;153m██████╔╝\033[38;5;117m██║   ██║\033[0m                          \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                          \033[38;5;219m| |_) | (_| | |_) | |_| | \033[38;5;189m██║   ██║\033[38;5;153m██╔═══╝ \033[38;5;117m██║   ██║\033[0m                          \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                          \033[38;5;225m|_.__/ \\__,_|_.__/ \\__, | \033[38;5;189m╚██████╔╝\033[38;5;153m██║     \033[38;5;117m╚██████╔╝\033[0m                          \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                          \033[38;5;225m                   |___/  \033[38;5;189m ╚═════╝ \033[38;5;153m╚═╝     \033[38;5;117m ╚═════╝ \033[0m                          \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                                                                                        \033[38;5;39m║\033[0m\n");
    printf(BOLD "  \033[38;5;39m║\033[0m                                                  \033[38;5;250m v0.1 \033[0m                                                \033[38;5;39m║\033[0m\n" RESET);
    printf("  \033[38;5;39m║\033[0m                                                                                                        \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                        \033[38;5;250m⚡ A Tiny GPU Simulator⚡\033[0m                                       \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                                                                                        \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                          \033[38;5;245m═╤═╤═╤═╤═╤═╤═╤═╤═╤═╤═╤═\033[0m                                       \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                         \033[38;5;245m╔╧═╧═╧═╧═╧═╧═╧═╧═╧═╧═╧═╧╗\033[0m                                      \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                       \033[38;5;245m══╣\033[0m \033[38;5;48m┌───────────────────┐\033[0m \033[38;5;245m╠══\033[0m                                    \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                       \033[38;5;245m══╣\033[0m \033[38;5;48m│\033[0m   \033[38;5;240m▪ ▪ ▪ ▪ ▪ ▪ ▪\033[0m   \033[38;5;48m│\033[0m \033[38;5;245m╠══\033[0m                                    \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                       \033[38;5;245m══╣\033[0m \033[38;5;48m│\033[0m   \033[38;5;240m▪ ▪ ▪ ▪ ▪ ▪ ▪\033[0m   \033[38;5;48m│\033[0m \033[38;5;245m╠══\033[0m                                    \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                       \033[38;5;245m══╣\033[0m \033[38;5;48m│\033[0m   \033[38;5;240m▪ ▪ ▪ ▪ ▪ ▪ ▪\033[0m   \033[38;5;48m│\033[0m \033[38;5;245m╠══\033[0m                                    \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                       \033[38;5;245m══╣\033[0m \033[38;5;48m│\033[0m   \033[38;5;240m▪ ▪ ▪ ▪ ▪ ▪ ▪\033[0m   \033[38;5;48m│\033[0m \033[38;5;245m╠══\033[0m                                    \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                       \033[38;5;245m══╣\033[0m \033[38;5;48m└───────────────────┘\033[0m \033[38;5;245m╠══\033[0m                                    \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                         \033[38;5;245m╚╤═╤═╤═╤═╤═╤═╤═╤═╤═╤═╤═╤╝\033[0m                                      \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                          \033[38;5;245m═╧═╧═╧═╧═╧═╧═╧═╧═╧═╧═╧═\033[0m                                       \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m║\033[0m                                                                                                        \033[38;5;39m║\033[0m\n");
    printf("  \033[38;5;39m╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝\033[0m\n");
    printf("\n");
}


void draw_main_menu(int selected, const char *message) {
    printf(CLEAR_SCREEN);
    print_header();
    printf("  \033[38;5;39m┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐\033[0m\n");
    printf("  \033[38;5;39m│\033[0m  \033[38;5;255m[\033[38;5;213m1\033[38;5;255m]\033[0m  Run a red-green flicker shader                                                                   \033[38;5;39m│\033[0m\n");
    printf("  \033[38;5;39m│\033[0m  \033[38;5;255m[\033[38;5;213m2\033[38;5;255m]\033[0m  Display a red square                                                                             \033[38;5;39m│\033[0m\n");
    printf("  \033[38;5;39m│\033[0m  \033[38;5;255m[\033[38;5;213m3\033[38;5;255m]\033[0m  Quit                                                                                             \033[38;5;39m│\033[0m\n");
    printf("  \033[38;5;39m└────────────────────────────────────────────────────────────────────────────────────────────────────────┘\033[0m\n");

    if (message) {
        printf("  " RED "⚠  %s" RESET "\n", message);
    }
    printf("\n  \033[38;5;250m❯\033[0m \033[38;5;48mEnter choice (1-3):\033[0m ");
    fflush(stdout);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    enable_ansi_support();

    int choice;
    int menu_running = 1;
    const char *error = NULL;

    while (menu_running) {
        draw_main_menu(0, error);

        if (scanf("%d", &choice) != 1) {
            // Handle non-numeric input
            while (getchar() != '\n');  // Clear input buffer
            error = "Please enter a number";
            continue;
        }
        switch (choice) {
            case 1:
                printf(CLEAR_SCREEN);
                printf("\nLaunching flicker application...\n");
                menu_running = 0;
                break;
            case 2:
                printf(CLEAR_SCREEN);
                printf("\nLaunching square application...\n");
                menu_running = 0;
                break;
            case 3:
                printf("\nExiting babyGPU\n");
                menu_running = 0;
                return 0;
                break;
            default:
                error = "Invalid choice (1-2 only)";
        }

    }

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

    // Flicker shader
    if(choice == 1) {
        int prog_id = gpu_load_program(dev, "flicker.shader");
        printf("Loaded program: flicker.shader\n");
        gpu_select_program(dev, prog_id);
        gpu_set_constant(dev, 0, 0xFF0000FF);  // iniate a color
    }

    // Square shader
    if (choice == 2) {
        int prog_id = gpu_load_program(dev, "square.shader");
        printf("Loaded program: square.shader\n");
        gpu_select_program(dev, prog_id);
        gpu_set_constant(dev, 0, 0xFF0000FF); // square color
        gpu_set_constant(dev, 1, 270); // square x
        gpu_set_constant(dev, 2, 370); // square x
        gpu_set_constant(dev, 3, 190); // square y
        gpu_set_constant(dev, 4, 290); // square y */     
    }

    bool running = true;

    int color_swap = 1; // only if program 1 is selected

    while(running) {
        int status = gpu_dispatch(dev);
        
        if (status != 0) {
            printf("Main Loop: GPU has tripped. Exiting.\n");
            running = false; 
            break;
        }

        // run our flicker shader (we need better app management later)
        if (choice == 1) {
            if (color_swap > 0) gpu_set_constant(dev, 0, 0xFF0000FF);
            else gpu_set_constant(dev, 0, 0xFF00FF00);
            color_swap *= -1;
        }

        if (!platform_update(gpu_get_vram(dev))) {
            running = false;
        }

    }

    platform_terminate();
    return 0;

}