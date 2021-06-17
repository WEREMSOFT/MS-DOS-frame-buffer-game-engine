#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "../graphics/graphics.h"
#include "../pixelUtils/sprite.h"

typedef struct
{
    Graphics graphics;
    Sprite background;
    Sprite selectionIndicator;
    int selectedShip;
} MainMenu;

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites);
void mainMenuUpdate(MainMenu this);
void mainMenuDestroy(MainMenu this);

#endif