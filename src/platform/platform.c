#include "platform.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")

static GLFWwindow* window = NULL;
static GLuint texture_id;
static int screen_width;
static int screen_height;

int platform_init(int width, int height, const char* title) {
    screen_width = width;
    screen_height = height;

    if (!glfwInit()) return 0;

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    // Setup the Texture
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Create empty texture buffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glEnable(GL_TEXTURE_2D);

    return 1;
}

int platform_update(uint32_t* buffer) {
    if (glfwWindowShouldClose(window)) return 0;

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screen_width, screen_height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL Error: 0x%04X\n", err);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
    glEnd();

    glfwSwapBuffers(window);
    glfwPollEvents();

    return 1;
}

void platform_terminate() {
    glfwTerminate();
}