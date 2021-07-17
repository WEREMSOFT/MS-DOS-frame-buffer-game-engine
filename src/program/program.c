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

Program programCreate()
{
    staticAllocatorInit(10092024);
    Program this = {0};
    this.graphics = graphicsCreate();
    // this.sound = soundCreate();
    // Soloud_setGlobalVolume(this.sound.soloud, 0);
    return this;
}

static void createImage(Graphics graphics, double time)
{
    double uvx;
    double uvy;

    int halfHeight = graphics.imageData.size.y / 2 + 1;
    int halfWidth = graphics.imageData.size.x / 2 + 1;

    for (int i = 0; i < halfWidth; i++)
    {
        for (int j = 0; j < halfHeight; j++)
        {
            uvx = (2.0 * i - graphics.imageData.size.x) / graphics.imageData.size.y;
            uvy = (2.0 * j - graphics.imageData.size.y) / graphics.imageData.size.y;

            uvy = 1.0 / fabsl(uvy);
            uvx *= uvy;
            uvx += time;
            uvy += time;

            Color color = (Color){
                sin(9.0 * uvx) * 255,
                sin(9.0 * uvy) * 255,
                sin(9.0 * uvy) * 255};

            unsigned int opositX = graphics.imageData.size.x - i;
            unsigned int opositY = graphics.imageData.size.y - j;

            imPutPixel(graphics.imageData, (PointI){i, j}, color);
            imPutPixel(graphics.imageData, (PointI){opositX, j}, color);
            imPutPixel(graphics.imageData, (PointI){opositX, opositY}, color);
            imPutPixel(graphics.imageData, (PointI){i, opositY}, color);
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
        lastUpdate = glfwGetTime();
        if (glfwGetKey(this.graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            shouldQuit = true;

        createImage(this.graphics, lastUpdate);
        printFPS(this.graphics, deltaTime);
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
    staticAllocatorDestroy();
}
