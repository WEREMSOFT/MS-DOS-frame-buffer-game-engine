#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "pixelUtils/graphics.h"
#include "pixelUtils/sprite.h"
#include "./array/array.h"

enum ProjectilesEnum
{
    PROJECTILE_HERO,
    PROJECTILE_COUNT
};

typedef struct
{
    Graphics graphics;
    Sprite background;
    Sprite projectiles[PROJECTILE_COUNT];
    PointF heroBulletsPositions[10];
    Sprite hero;
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif