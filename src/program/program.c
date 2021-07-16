#include "program.h"
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

#ifdef ARRAY_MALLOC
#undef ARRAY_MALLOC
#endif

#ifdef ARRAY_REALLOC
#undef ARRAY_REALLOC
#endif

#define ARRAY_MALLOC allocStatic

#define ARRAY_REALLOC reallocStatic

#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "core/array/array.h"

#define STBI_MALLOC(sz) allocStatic(sz)
#define STBI_REALLOC(p, newsz) reallocStatic(p, newsz)
#define STBI_FREE(p) freeStatic(p)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Program
programCreate()
{
    staticAllocatorInit(100092024);
    Program this = {0};
    this.graphics = graphicsCreate();
    this.sound = soundCreate();
    Soloud_setGlobalVolume(this.sound.soloud, 0);
    return this;
}

static void createImage(Graphics graphics, double deltaTime)
{
    unsigned int uvx;
    unsigned int uvy;

    for (int i = 0; i < graphics.imageData.size.x; i++)
    {
        for (int j = 0; j < graphics.imageData.size.y; j++)
        {
            uvx = (2.0 * i - graphics.imageData.size.x) / graphics.imageData.size.y;
            uvy = (2.0 * j - graphics.imageData.size.y) / graphics.imageData.size.y;
            imPutPixel(graphics.imageData, (PointI){i, j}, (Color){i % 255, j % 255, (i * j) % 255});
        }
    }
}

void programMainLoop(Program this)
{
    bool shouldQuit = false;
    double lastUpdate = glfwGetTime();
    while (!shouldQuit)
    {
        double deltaTime = glfwGetTime() - lastUpdate;
        if (glfwGetKey(this.graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            shouldQuit = true;

        imClear(this.graphics.imageData);

        createImage(this.graphics, deltaTime);

        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
        lastUpdate = glfwGetTime();
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
    staticAllocatorDestroy();
}
