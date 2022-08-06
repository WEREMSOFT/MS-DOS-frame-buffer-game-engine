#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "../core/graphics/graphics.h"
#include "../core/sprite/sprite.h"
#include "../sound/sound.h"

typedef enum
{
    QPOS_NONE,
    QPOS_TOP,
    QPOS_BOTTOM,
    QPOS_LEFT,
    QPOS_RIGHT,
    QPOS_TOP_RIGHT,
    QPOS_TOP_LEFT,
    QPOS_BOTTOM_RIGHT,
    QPOS_BOTTOM_LEFT,
    QPOS_COUNT
} QuadrantPosition;

typedef enum
{
    ENEMY_STATE_IDLE,
    ENEMY_STATE_GOING_UP,
    ENEMY_STATE_GOING_DOWN,
    ENEMY_STATE_DEAD,
    ENEMY_STATE_COUNT
} EnemyState;

typedef struct
{
    int spriteId;
    PointI basePosition;
    EnemyState state;
} EnemyBig;

typedef struct
{
    Graphics graphics;
    Sprite background;
    Sprite enemyGreenBig;
    Sprite sight;
    Sprite shoot;
    PointI textPosition;
    QuadrantPosition quadPosition;
    Sound sound;
    bool shouldQuit;
    EnemyBig enemyBigs[8];
    PointI positions[QPOS_COUNT];
} MainMenu;

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites, Sound sound);
MainMenu mainMenuUpdate(MainMenu this);

#endif