#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include "core/graphics/graphics.h"
#include "core/sprite/sprite.h"
#include "level1/level1.h"
#include "assetManager/assetManager.h"
#include "sound/sound.h"

typedef struct
{
    Graphics graphics;
    Level1 mainMenu;
    Sprite sprites[ASSET_COUNT];
    Sound sound;
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif