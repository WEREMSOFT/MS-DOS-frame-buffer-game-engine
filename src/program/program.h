#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "pixelUtils/graphics.h"
#include "pixelUtils/sprite.h"
#include "./array/array.h"

typedef struct
{
    Graphics graphics;
    Sprite background;
    Sprite selectedIsoTile;
    Array *isoTiles;
    Array *sprites;
    PointI tileSize;
    int selectedIsoTileIndex;
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif