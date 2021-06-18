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
    int x;
    int y;
} PointI;

typedef struct
{
    double x;
    double y;
} PointF;

typedef struct
{
    unsigned int textureId;
    PointI screenSize;
    unsigned int VAO;
    unsigned int shaderProgram;
    PointI mousePosition;
    bool mouseRightDown;
    GLFWwindow *window;
    Color *textureData;
} Graphics;

Graphics graphicsCreate();
void graphicsSwapBuffers(Graphics this);
void graphicsDestroy(Graphics this);
void graphicsUpdateMouseCoordinates(Graphics *this);
void graphicsPutPixel(Graphics this, PointI point, Color color);
void graphicsDrawCircle(Graphics this, PointI center, double radious, Color color);
void graphicsDrawCircleFill(Graphics this, PointI center, double radious, Color color);
void graphicsDrawSquare(Graphics this, PointI topLeftCorner, PointI size, Color color);
void graphicsClear(Graphics this);
void graphicsDrawCharacter(Graphics this, PointI topLeftCorner, unsigned int letter, Color color);
void graphicsPrintFontTest(Graphics this);
void graphicsPrintString(Graphics this, PointI topLeftCorner, char *string, Color color);
void graphicsDrawLine(Graphics this, PointI pointA, PointI pointB, Color color);
#endif