#include "program.h"

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "core/input/input.h"
#include "utils/utils.h"
#include "assetManager/assetManager.h"

#define __STATIC_ALLOC_IMPLEMENTATION__
#include "core/stackAllocator/staticAlloc.h"

#define ARRAY_MALLOC allocStatic
#define ARRAY_REALLOC reallocStatic
#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "core/array/array.h"

#define STBI_MALLOC(sz) allocStatic(sz)
#define STBI_REALLOC(p, newsz) reallocStatic(p, newsz)
#define STBI_FREE(p) freeStatic(p)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Program programCreate()
{
    staticAllocatorInit(100092024);
    Program this = {0};
    this.graphics = graphicsCreate(320, 240);
    this.sound = soundCreate();
    spritesLoad(this.sprites);
    Soloud_setGlobalVolume(this.sound.soloud, 0);
    this.mainMenu = mainMenuCreate(this.graphics, this.sprites, this.sound);

    return this;
}

void programMainLoop(Program this)
{
    while (!this.level.shouldQuit)
    {
        this.mainMenu = mainMenuUpdate(this.mainMenu);
        if (this.mainMenu.shouldQuit)
            return;
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
    staticAllocatorDestroy();
}
