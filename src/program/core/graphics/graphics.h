#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "imageData/imageData.h"

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

Graphics graphicsCreate();
void graphicsSwapBuffers(Graphics this);
void graphicsDestroy(Graphics this);
void graphicsUpdateMouseCoordinates(Graphics *this);
void graphicsPrintString(Graphics this, PointI topLeftCorner, char *string, Color color);
#endif