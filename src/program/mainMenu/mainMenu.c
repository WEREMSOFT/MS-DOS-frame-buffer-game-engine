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
    this.logo = sprites[ASSET_LOGO];
    this.logo.position.x = (this.graphics.screenSize.x - this.logo.size.x) * 0.5;
    this.logo.position.y = 20;

    this.ships[SHIP_BLUE] = sprites[ASSET_SHIP_BLUE];
    this.ships[SHIP_RED] = sprites[ASSET_SHIP_RED];
    this.ships[SHIP_GREEN] = sprites[ASSET_SHIP_GREEN];

    int gap = 10;

    this.ships[SHIP_RED].position.x = this.graphics.screenSize.x * 0.5 - (this.ships[SHIP_RED].size.x + this.ships[SHIP_GREEN].size.x + this.ships[SHIP_BLUE].size.x + gap * 2) * 0.5;
    this.ships[SHIP_RED].position.y = this.logo.position.y + this.logo.size.y + 30;

    this.ships[SHIP_GREEN].position.x = this.ships[SHIP_RED].position.x + this.ships[SHIP_RED].size.x + gap;
    this.ships[SHIP_GREEN].position.y = this.logo.position.y + this.logo.size.y + 30;

    this.ships[SHIP_BLUE].position.x = this.ships[SHIP_GREEN].position.x + this.ships[SHIP_GREEN].size.x + gap;
    this.ships[SHIP_BLUE].position.y = this.logo.position.y + this.logo.size.y + 30;

    this.textPosition.x = this.graphics.screenSize.x / 2 - 32;
    this.textPosition.y = this.logo.position.y + this.logo.size.y + 10;
    this.background = spriteCreateCkeckerBoard(this.graphics, (PointI){800, 600}, 20, (Color){159, 194, 214}, (Color){103, 146, 171});

    this.shouldQuit = false;
    return this;
}
MainMenu mainMenuUpdate(MainMenu this)
{
    bool shouldContinue = true;
    while (shouldContinue && !this.shouldQuit)
    {
        graphicsClear(this.graphics);
        shouldContinue = !isKeyJustPressed(this.graphics.window, GLFW_KEY_ENTER);
        this.shouldQuit = isKeyJustPressed(this.graphics.window, GLFW_KEY_ESCAPE);

        this.selectedShip += isKeyJustPressed(this.graphics.window, GLFW_KEY_RIGHT);
        this.selectedShip -= isKeyJustPressed(this.graphics.window, GLFW_KEY_LEFT);

        this.selectedShip = fmax(0, fmin(this.selectedShip, SHIP_COUNT - 1));

        this.background.position = positionUpdateIntoCircularMovenent();
        spriteDrawClipped(this.background, this.graphics);
        spriteDrawTransparent(this.logo, this.graphics);

        Color selectionColor = (Color){random() % 255, random() % 255, random() % 255};

        for (int i = 0; i < SHIP_COUNT; i++)
        {
            spriteDrawTransparent(this.ships[i], this.graphics);
        }

        graphicsDrawSquare(this.graphics, (PointI){this.ships[this.selectedShip].position.x, this.ships[this.selectedShip].position.y}, this.ships[this.selectedShip].size, selectionColor);
        graphicsPrintString(this.graphics, this.textPosition, "select ship", selectionColor);

        printFPS(this.graphics, getDeltaTime());
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }
    return this;
}