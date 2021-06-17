#include <stdio.h>
#include <stdbool.h>

#define __ASSET_MANAGER_IMPLEMENTATION__
#include "program/assetManager/assetManager.h"
#include "program/program.h"

#define __STATIC_ALLOC_IMPLEMENTATION__
#include "program/stackAllocator/staticAlloc.h"

#define ARRAY_MALLOC allocStatic
#define ARRAY_REALLOC reallocStatic
#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "program/array/array.h"

#define STBI_MALLOC(sz) allocStatic(sz)
#define STBI_REALLOC(p, newsz) reallocStatic(p, newsz)
#define STBI_FREE(p) freeStatic(p)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "program/mainMenu/mainMenu.h"
#include "program/assetManager/assetManager.h"
int main(void)
{
    staticAllocatorInit();

    Sprite sprites[ASSET_COUNT];
    spritesInit(sprites);

    Graphics graphics = graphicsCreate();
    Program program = programCreate(graphics, sprites);
    MainMenu mainMenu = mainMenuCreate(graphics, sprites);

    mainMenuUpdate(mainMenu);
    programMainLoop(program);

    programDestroy(program);
    mainMenuDestroy(mainMenu);
    graphicsDestroy(graphics);
    staticAllocatorDestroy();
    return 0;
}