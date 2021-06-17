#include "mainMenu.h"
#include <stdlib.h>
#include "../graphics/graphics.h"
#include "../utils/utils.h"
#include "../input/keyboard.h"
#include "../pixelUtils/sprite.h"
#include "../assetManager/assetManager.h"

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites)
{
    MainMenu this = {0};
    this.graphics = graphics;
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
        graphicsPrintString(this.graphics, (PointI){120, 51}, "select ship", (Color){random() % 255, random() % 255, random() % 255});
        printFPS(this.graphics, getDeltaTime());
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }
}
void mainMenuDestroy(MainMenu this) {}