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

typedef struct
{
    int x;
    int y;
} Pointi;

typedef struct
{
    double x;
    double y;
} Pointf;

Graphics graphicsCreate();
void graphicsSwapBuffers(Graphics this);
void graphicsDestroy(Graphics this);
void graphicsUpdateMouseCoordinates(Graphics *this);
void graphicsPutPixel(Graphics this, Pointi point, Color color);
void graphicsDrawCircle(Graphics this, Pointi center, double radious, Color color);
void graphicsDrawCircleFill(Graphics this, Pointi center, double radious, Color color);
void graphicsDrawSquare(Graphics this, Pointi topLeftCorner, Pointi size, Color color);
void graphicsClear(Graphics this);
void graphicsDrawCharacter(Graphics this, Pointi topLeftCorner, unsigned int letter, Color color);
void graphicsPrintFontTest(Graphics this);
void graphicsPrintString(Graphics this, Pointi topLeftCorner, char *string, Color color);
void graphicsDrawLine(Graphics this, Pointi pointA, Pointi pointB, Color color);
#endif