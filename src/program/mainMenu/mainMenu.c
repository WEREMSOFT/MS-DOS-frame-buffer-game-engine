#include "mainMenu.h"
#include <stdlib.h>
#include "../core/graphics/graphics.h"
#include "../utils/utils.h"
#include "../core/input/keyboard.h"
#include "../core/sprite/sprite.h"
#include "../assetManager/assetManager.h"

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites, Sound sound)
{
    MainMenu this = {0};
    this.sound = sound;
    this.graphics = graphics;
    this.logo = sprites[ASSET_LOGO];
    this.logo.position.x = (this.graphics.imageData.size.x - this.logo.imageData.size.x) * 0.5;
    this.logo.position.y = 20;

    this.ships[SHIP_BLUE] = sprites[ASSET_SHIP_BLUE];
    this.ships[SHIP_RED] = sprites[ASSET_SHIP_RED];
    this.ships[SHIP_GREEN] = sprites[ASSET_SHIP_GREEN];

    int gap = 10;

    this.ships[SHIP_RED].position.x = this.graphics.imageData.size.x * 0.5 - (this.ships[SHIP_RED].imageData.size.x + this.ships[SHIP_GREEN].imageData.size.x + this.ships[SHIP_BLUE].imageData.size.x + gap * 2) * 0.5;
    this.ships[SHIP_RED].position.y = this.logo.position.y + this.logo.imageData.size.y + 30;

    this.ships[SHIP_GREEN].position.x = this.ships[SHIP_RED].position.x + this.ships[SHIP_RED].imageData.size.x + gap;
    this.ships[SHIP_GREEN].position.y = this.logo.position.y + this.logo.imageData.size.y + 30;

    this.ships[SHIP_BLUE].position.x = this.ships[SHIP_GREEN].position.x + this.ships[SHIP_GREEN].imageData.size.x + gap;
    this.ships[SHIP_BLUE].position.y = this.logo.position.y + this.logo.imageData.size.y + 30;

    this.textPosition.x = this.graphics.imageData.size.x / 2 - 32;
    this.textPosition.y = this.logo.position.y + this.logo.imageData.size.y + 10;
    this.background = spriteCreateCkeckerBoard((PointI){800, 600}, 20, (Color){159, 194, 214}, (Color){103, 146, 171});

    this.shouldQuit = false;
    return this;
}
MainMenu mainMenuUpdate(MainMenu this)
{
    soundPlaySpeech(this.sound, SPEECH_SELECT_SHIP);

    bool shouldContinue = true;
    int lastSelectedShip = this.selectedShip;
    while (shouldContinue && !this.shouldQuit)
    {
        imClear(this.graphics.imageData);
        if (isKeyJustPressed(this.graphics.window, GLFW_KEY_ENTER))
        {
            shouldContinue = false;
            soundPlaySfx(this.sound, SFX_SELECT);
        }
        this.shouldQuit = isKeyJustPressed(this.graphics.window, GLFW_KEY_ESCAPE);

        this.selectedShip += isKeyJustPressed(this.graphics.window, GLFW_KEY_RIGHT);
        this.selectedShip -= isKeyJustPressed(this.graphics.window, GLFW_KEY_LEFT);

        this.selectedShip = (SHIP_COUNT + this.selectedShip) % SHIP_COUNT;

        if (this.selectedShip != lastSelectedShip)
        {
            soundPlaySfx(this.sound, SFX_BLIP);
        }

        lastSelectedShip = this.selectedShip;

        this.background.position = positionUpdateIntoCircularMovenent();
        spriteDrawClipped(this.background, this.graphics.imageData);
        spriteDrawTransparent(this.logo, this.graphics.imageData);

        Color selectionColor = (Color){random() % 255, random() % 255, random() % 255};

        for (int i = 0; i < SHIP_COUNT; i++)
        {
            spriteDrawTransparent(this.ships[i], this.graphics.imageData);
        }

        imDrawSquare(this.graphics.imageData, (PointI){this.ships[this.selectedShip].position.x, this.ships[this.selectedShip].position.y}, this.ships[this.selectedShip].imageData.size, selectionColor);
        imPrintString(this.graphics.imageData, this.textPosition, "select ship", selectionColor);

        printFPS(this.graphics, getDeltaTime());
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }

    return this;
}