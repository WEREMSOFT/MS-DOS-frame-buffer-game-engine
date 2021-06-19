#include "mainMenu.h"
#include <stdlib.h>
#include "../core/graphics/graphics.h"
#include "../utils/utils.h"
#include "../core/input/keyboard.h"
#include "../core/sprite/sprite.h"
#include "../assetManager/assetManager.h"
#include <soloud_c.h>

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
    Soloud *soloud = Soloud_create();
    Speech *speechSelectYourShip = Speech_create();
    Sfxr *sfxr = Sfxr_create();
    Sfxr *selectionSound = Sfxr_create();

    Speech_setText(speechSelectYourShip, "select your ship");
    Sfxr_loadPreset(sfxr, SFXR_BLIP, 3247);
    Sfxr_loadPreset(selectionSound, SFXR_POWERUP, 3247);

    Soloud_initEx(soloud, SOLOUD_CLIP_ROUNDOFF | SOLOUD_ENABLE_VISUALIZATION,
                  SOLOUD_AUTO, SOLOUD_AUTO, SOLOUD_AUTO, 2);

    Soloud_setGlobalVolume(soloud, 4);

    Soloud_setRelativePlaySpeed(soloud, Soloud_play(soloud, speechSelectYourShip), 1.0);

    bool shouldContinue = true;
    int lastSelectedShip = this.selectedShip;
    while (shouldContinue && !this.shouldQuit)
    {
        graphicsClear(this.graphics);
        if (isKeyJustPressed(this.graphics.window, GLFW_KEY_ENTER))
        {
            shouldContinue = false;
            Soloud_play(soloud, selectionSound);
        }
        this.shouldQuit = isKeyJustPressed(this.graphics.window, GLFW_KEY_ESCAPE);

        this.selectedShip += isKeyJustPressed(this.graphics.window, GLFW_KEY_RIGHT);
        this.selectedShip -= isKeyJustPressed(this.graphics.window, GLFW_KEY_LEFT);

        this.selectedShip = (SHIP_COUNT + this.selectedShip) % SHIP_COUNT;

        if (this.selectedShip != lastSelectedShip)
        {
            Soloud_play(soloud, sfxr);
        }

        lastSelectedShip = this.selectedShip;

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

    Sfxr_destroy(selectionSound);
    Sfxr_destroy(sfxr);
    Soloud_deinit(soloud);
    Speech_destroy(speechSelectYourShip);
    Soloud_destroy(soloud);
    return this;
}