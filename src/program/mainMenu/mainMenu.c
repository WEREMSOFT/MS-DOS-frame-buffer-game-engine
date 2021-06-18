#include "mainMenu.h"
#include <stdlib.h>
#include "../core/graphics/graphics.h"
#include "../utils/utils.h"
#include "../core/input/keyboard.h"
#include "../core/sprite/sprite.h"
#include "../assetManager/assetManager.h"

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites)
{
    MainMenu this = {0};
    this.graphics = graphics;
    this.sprites = sprites;
    this.background = spriteCreateCkeckerBoard(this.graphics, (PointI){800, 600}, 20, (Color){0, 0xA0, 0xB0}, (Color){0xED, 0xC9, 0x51});
    return this;
}
void mainMenuUpdate(MainMenu this)
{
    bool shouldContinue = true;
    while (shouldContinue)
    {
        graphicsClear(this.graphics);
        if (isKeyJustPressed(this.graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            shouldContinue = false;
        }
        this.background.position = positionUpdateIntoCircularMovenent();
        spriteDrawClipped(this.background, this.graphics);
        this.sprites[ASSET_SHIP_BLUE].position.x = 160;
        this.sprites[ASSET_SHIP_BLUE].position.y = 100;
        spriteDrawTransparent(this.sprites[ASSET_SHIP_BLUE], this.graphics);
        graphicsPrintString(this.graphics, (PointI){120, 51}, "select ship", (Color){random() % 255, random() % 255, random() % 255});
        printFPS(this.graphics, getDeltaTime());
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }
}
void mainMenuDestroy(MainMenu this) {}