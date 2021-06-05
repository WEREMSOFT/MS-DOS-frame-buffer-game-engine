#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "pixelUtils/graphics.h"

typedef struct
{
    Graphics graphics;
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif