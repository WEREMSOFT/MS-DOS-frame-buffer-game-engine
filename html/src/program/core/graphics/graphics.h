#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
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
    PointI size;
    int bufferSize;
    Color *data;
} ImageData;

typedef struct
{
    unsigned int textureId;
    unsigned int VAO;
    unsigned int shaderProgram;
    PointI mousePosition;
    bool mouseRightDown;
    GLFWwindow *window;
    ImageData imageData;
} Graphics;

Graphics graphicsCreate(int width, int height, bool fullScreen);
void graphicsSwapBuffers(Graphics this);
void graphicsDestroy(Graphics this);
void graphicsUpdateMouseCoordinates(Graphics *this);
void graphicsPutPixel(ImageData this, PointI point, Color color);
void graphicsDrawCircle(ImageData this, PointI center, double radious, Color color);
void graphicsDrawCircleFill(ImageData this, PointI center, double radious, Color color);
void graphicsDrawSquare(ImageData this, PointI topLeftCorner, PointI size, Color color);
void graphicsDrawSquareFill(ImageData this, PointI topLeftCorner, PointI size, Color color);
void graphicsClear(ImageData this);
void graphicsDrawCharacter(ImageData this, PointI topLeftCorner, unsigned int letter, Color color);
void graphicsPrintFontTest(ImageData this);
void graphicsPrintString(ImageData this, PointI topLeftCorner, char *string, Color color);
void graphicsDrawLine(ImageData this, PointI pointA, PointI pointB, Color color);
Color graphicsGetPixel(ImageData this, PointI point);
PointI pointFToPointI(PointF);
#endif