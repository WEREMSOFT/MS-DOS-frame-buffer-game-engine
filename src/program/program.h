#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include "core/graphics/graphics.h"
#include "core/sprite/sprite.h"
#include "mainMenu/mainMenu.h"
#include "assetManager/assetManager.h"
#include "sound/sound.h"

typedef struct
{
    Graphics graphics;
    MainMenu mainMenu;
    Sprite sprites[ASSET_COUNT];
    Sound sound;
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif