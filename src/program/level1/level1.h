#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "../core/graphics/graphics.h"
#include "../core/sprite/sprite.h"
#include "../sound/sound.h"
#include "enemy.h"

typedef enum
{
    QPOS_NONE,
    QPOS_BOTTOM,
    QPOS_BOTTOM_RIGHT,
    QPOS_BOTTOM_LEFT,
    QPOS_TOP,
    QPOS_TOP_RIGHT,
    QPOS_TOP_LEFT,
    QPOS_RIGHT,
    QPOS_LEFT,
    QPOS_COUNT
} QuadrantPosition;

typedef struct
{
    Graphics graphics;
    Sprite background;
    Sprite enemyGreenBig;
    Sprite enemyGreenSmall;
    Sprite sight;
    Sprite shoot;
    PointI textPosition;
    QuadrantPosition quadPosition;
    Sound sound;
    bool shouldQuit;
    Enemy enemyBigs[8];
    PointI positions[QPOS_COUNT];
} MainMenu;

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites, Sound sound);
MainMenu mainMenuUpdate(MainMenu this);

#endif