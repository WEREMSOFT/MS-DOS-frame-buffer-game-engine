#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "../core/graphics/graphics.h"
#include "../core/sprite/sprite.h"
#include "../sound/sound.h"

typedef enum
{
    SHIP_RED,
    SHIP_GREEN,
    SHIP_BLUE,
    SHIP_COUNT
} ShipsEnum;

typedef enum
{
    AIM_TOP,
    AIM_BOTTOM,
    AIM_LEFT,
    AIM_RIGHT,
    AIM_UP_RIGHT,
    AIM_UP_LEFT,
    AIM_DOWN_RIGHT,
    AIM_DOWN_LEFT,
    AIM_COUNT
} AimState;

typedef struct
{
    Graphics graphics;
    Sprite background;
    Sprite sight;
    Sprite shoot;
    PointI textPosition;
    AimState aimState;
    Sound sound;
    bool shouldQuit;
} MainMenu;

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites, Sound sound);
MainMenu mainMenuUpdate(MainMenu this);

#endif