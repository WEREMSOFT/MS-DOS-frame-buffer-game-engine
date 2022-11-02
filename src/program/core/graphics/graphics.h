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
void graphicsClear(ImageData this);
Color graphicsGetPixel(ImageData this, PointI point);
#endif