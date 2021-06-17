#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "graphics/graphics.h"
#include "pixelUtils/sprite.h"

#define MAX_HERO_BULLETS_ON_SCREEN 10

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
    PointF heroBulletsPositions[MAX_HERO_BULLETS_ON_SCREEN];
    Sprite hero;
} Program;

Program programCreate(Graphics graphics, Sprite *sprites);
void programMainLoop(Program this);
void programDestroy(Program this);

#endif