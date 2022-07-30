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

    this.background = sprites[ASSET_BACKGROUND];

    this.shouldQuit = false;
    return this;
}
MainMenu mainMenuUpdate(MainMenu this)
{
    soundPlaySpeech(this.sound, SPEECH_SELECT_SHIP);

    bool shouldContinue = true;

    while (shouldContinue && !this.shouldQuit)
    {
        graphicsClear(this.graphics.imageData);
        if (isKeyJustPressed(this.graphics.window, GLFW_KEY_ENTER))
        {
            shouldContinue = false;
            soundPlaySfx(this.sound, SFX_SELECT);
        }
        this.shouldQuit = isKeyJustPressed(this.graphics.window, GLFW_KEY_ESCAPE);

        spriteDrawClipped(this.background, this.graphics.imageData);

        printFPS(this.graphics, getDeltaTime());
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }

    return this;
}