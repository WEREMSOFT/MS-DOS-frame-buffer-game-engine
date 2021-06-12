#include "program.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <math.h>
#include "input/input.h"
#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "array/array.h"
Program programCreate()
{
    Program this = {0};
    this.graphics = graphicsCreate();
    this.isoTile = spriteCreate("assets/iso_tile.png");
    this.background = spriteCreate("assets/320x200.png");

    this.sprites = arrayCreate(10, sizeof(Sprite));

    return this;
}

void printFPS(Graphics this)
{
    static double lastUpdate;
    static double currentFPS;

    currentFPS = (currentFPS + 1 / (glfwGetTime() - lastUpdate)) / 2;
    {
        char text[1000] = {0};
        snprintf(text, 1000, "fps: %d", (int)floor(currentFPS));
        graphicsPrintString(this, (Pointi){100, 0}, text, (Color){0, 0xff, 0xff});
    }
    {
        graphicsDrawCircle(this, this.mousePosition, this.mouseRightDown ? 2 : 4, this.mouseRightDown ? (Color){255, 0, 0} : (Color){0, 255, 0});
        graphicsPutPixel(this, this.mousePosition, (Color){255, 255, 255});
    }
    lastUpdate = glfwGetTime();
}

void programMainLoop(Program this)
{
    Graphics graphics = this.graphics;

    while (!glfwWindowShouldClose(this.graphics.window))
    {
        graphicsUpdateMouseCoordinates(&graphics);
        graphicsClear(this.graphics);

        if (glfwGetKey(graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(graphics.window, true);
        }

        if (isMouseButtonJustPressed(graphics.window, GLFW_MOUSE_BUTTON_1))
        {
            arrayInsertElement(&this.sprites, &this.isoTile);
        }

        spriteDraw(this.background, graphics);
        this.isoTile.position = (Pointi){
            (int)(graphics.mousePosition.x - graphics.mousePosition.x % (int)ceil(this.isoTile.size.x * 0.5)),
            (int)(graphics.mousePosition.y - graphics.mousePosition.y % (int)ceil(this.isoTile.size.y * 0.5))};
        spriteDrawTransparent(this.isoTile, graphics);

        for (int i = 0; i < this.sprites->header.length; i++)
        {
            Sprite *sprite = arrayGetElementAt(this.sprites, i);
            spriteDrawTransparent(*sprite, graphics);
        }

        printFPS(graphics);
        graphicsSwapBuffers(graphics);
        glfwPollEvents();
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
}