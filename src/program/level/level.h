#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "../core/graphics/graphics.h"
#include "../core/sprite/sprite.h"

#define MAX_HERO_BULLETS_ON_SCREEN 10

typedef struct
{
    double x, y, z;
} Star;

enum ProjectilesEnum
{
    PROJECTILE_HERO,
    PROJECTILE_COUNT
};

typedef struct
{
    Graphics graphics;
    Sprite projectiles[PROJECTILE_COUNT];
    PointF heroBulletsPositions[MAX_HERO_BULLETS_ON_SCREEN];
    Sprite hero;
} Level;

Level levelCreate(Graphics graphics, Sprite *sprites);
void levelMainLoop(Level this);
void levelDestroy(Level this);

#endif