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
    this.sprites = arrayCreate(10, sizeof(Sprite));
    this.isoTiles = arrayCreate(10, sizeof(Sprite));
    {
        Sprite tile = spriteCreate("assets/tiles/iso_tile_0.png");
        this.tileSize = tile.size;
        arrayInsertElement(&this.isoTiles, &tile);

        tile = spriteCreate("assets/tiles/iso_tile_1.png");
        arrayInsertElement(&this.isoTiles, &tile);

        tile = spriteCreate("assets/tiles/iso_tile_2.png");
        arrayInsertElement(&this.isoTiles, &tile);

        tile = spriteCreate("assets/tiles/iso_tile_3.png");
        arrayInsertElement(&this.isoTiles, &tile);
    }
    this.selectedIsoTile = *(Sprite *)arrayGetElementAt(this.isoTiles, this.selectedIsoTileIndex);
    this.background = spriteCreateCkeckerBoard(this.graphics, 10, (Color){0xFE, 0xF0, 0xA5}, (Color){0x74, 0x81, 0x9D});
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
        graphicsPrintString(this, (PointI){100, 0}, text, (Color){0, 0xff, 0xff});
    }
    {
        graphicsDrawCircle(this, this.mousePosition, this.mouseRightDown ? 2 : 4, this.mouseRightDown ? (Color){255, 0, 0} : (Color){0, 255, 0});
        graphicsPutPixel(this, this.mousePosition, (Color){255, 255, 255});
    }
    lastUpdate = glfwGetTime();
}

int x = 0;

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

        if (isKeyJustPressed(graphics.window, GLFW_KEY_SPACE))
        {
            this.selectedIsoTileIndex++;
            this.selectedIsoTileIndex %= this.isoTiles->header.length;
            this.selectedIsoTile = *(Sprite *)arrayGetElementAt(this.isoTiles, this.selectedIsoTileIndex);
        }

        if (isMouseButtonJustPressed(graphics.window, GLFW_MOUSE_BUTTON_1))
        {
            arrayInsertElement(&this.sprites, &this.selectedIsoTile);
        }

        if (isKeyJustPressed(graphics.window, GLFW_KEY_RIGHT))
        {
            x++;
        }

        spriteDraw(this.background, graphics);
        graphicsPutPixel(graphics, (PointI){x, 0}, (Color){0xff, 0, 0});

        for (int i = 0; i < this.sprites->header.length; i++)
        {
            Sprite *sprite = arrayGetElementAt(this.sprites, i);
            spriteDrawTransparent(*sprite, graphics);
        }

        this.selectedIsoTile.position = (PointF){
            (int)(graphics.mousePosition.x - graphics.mousePosition.x % (int)ceil(this.tileSize.x * 0.5)) - 10,
            (int)(graphics.mousePosition.y - graphics.mousePosition.y % (int)ceil(this.tileSize.y * 0.5)) - 10};

        this.selectedIsoTile.position.y -= this.selectedIsoTile.size.y - this.tileSize.y;
        spriteDrawClipped(this.selectedIsoTile, graphics);

        printFPS(graphics);
        graphicsSwapBuffers(graphics);
        glfwPollEvents();
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
}