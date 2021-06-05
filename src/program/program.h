#ifndef __PROGRAM_H__
#define __PROGRAM___
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define TEXTURE_WIDTH 320.0
#define TEXTURE_HEIGHT 200.0

typedef struct
{
    unsigned char r, g, b;
} Color;

typedef struct
{
    GLFWwindow *window;
    unsigned int textureId;
    unsigned int shaderProgram;
    unsigned int VAO;
    Color *textureData;
} Program;

Program programCreate();
void programMainLoop(Program program);
void programDestroy(Program program);

#endif