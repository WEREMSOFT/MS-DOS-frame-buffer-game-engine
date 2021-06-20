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

typedef struct
{
    Graphics graphics;
    Sprite background;
    Sprite selectionIndicator;
    Sprite ships[SHIP_COUNT];
    Sprite logo;
    int selectedShip;
    PointI textPosition;
    Sound sound;
    bool shouldQuit;
} MainMenu;

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites, Sound sound);
MainMenu mainMenuUpdate(MainMenu this);

#endif