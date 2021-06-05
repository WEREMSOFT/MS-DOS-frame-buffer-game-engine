#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct
{
    unsigned char r, g, b;
} Color;

typedef struct
{
    unsigned int textureId;
    unsigned int textureWidth;
    unsigned int textureHeight;
    unsigned int VAO;
    unsigned int shaderProgram;
    GLFWwindow *window;
    Color *textureData;
} Graphics;

Graphics graphicsCreate();
void graphicsSwapBuffers(Graphics this);
void graphicsDestroy(Graphics this);

void graphicsPutPixel(Graphics this, int x, int y, Color color);
void graphicsDrawCircle(Graphics this, double x, double y, double radious, Color color);
void graphicsDrawSquare(Graphics this, double x, double y, double width, double height, Color color);
void textureDataClear(Graphics this);
#endif