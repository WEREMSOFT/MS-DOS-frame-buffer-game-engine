#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

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
    double mouseX;
    double mouseY;
    bool mouseRightDown;
    GLFWwindow *window;
    Color *textureData;
} Graphics;

Graphics graphicsCreate();
void graphicsSwapBuffers(Graphics this);
void graphicsDestroy(Graphics this);
void graphicsUpdateMouseCoordinates(Graphics *this);
void graphicsPutPixel(Graphics this, int x, int y, Color color);
void graphicsDrawCircle(Graphics this, double x, double y, double radious, Color color);
void graphicsDrawSquare(Graphics this, double x, double y, double width, double height, Color color);
void graphicsClear(Graphics this);
void graphicsDrawCharacter(Graphics this, double x, double y, unsigned int letter, Color color);
void graphicsPrintFontTest(Graphics this);
void graphicsPrintString(Graphics this, int x, int y, char *string, Color color);
#endif