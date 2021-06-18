#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/graphics/graphics.h"
#include "core/sprite/sprite.h"
#include "level/level.h"
#include "mainMenu/mainMenu.h"
#include "assetManager/assetManager.h"

typedef struct
{
    Graphics graphics;
    Level level;
    MainMenu mainMenu;
    Sprite sprites[ASSET_COUNT];
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif