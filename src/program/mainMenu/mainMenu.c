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
    this.shoot = sprites[ASSET_SHOOT];
    this.sight = sprites[ASSET_SIGHT];
    this.shouldQuit = false;
    return this;
}

MainMenu handleControls(MainMenu this)
{
    if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        this.sight.position.x = 42;
        this.sight.position.y = 50;
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.aimState = AIM_UP_LEFT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        this.sight.position.x = 260;
        this.sight.position.y = 50;
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.aimState = AIM_UP_RIGHT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        this.sight.position.x = 55;
        this.sight.position.y = 180;
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.aimState = AIM_DOWN_LEFT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        this.sight.position.x = 220;
        this.sight.position.y = 180;
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.aimState = AIM_DOWN_RIGHT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        this.sight.position.x = 120;
        this.sight.position.y = 50;
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.aimState = AIM_TOP;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        this.sight.position.x = 120;
        this.sight.position.y = 180;
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.aimState = AIM_BOTTOM;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        this.sight.position.x = 260;
        this.sight.position.y = 130;
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.aimState = AIM_RIGHT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        this.sight.position.x = 55;
        this.sight.position.y = 120;
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.aimState = AIM_LEFT;
        return this;
    }
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
        this = handleControls(this);

        if (glfwGetKey(this.graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
            switch (this.aimState)
            {
            case AIM_TOP:
                spriteDrawTransparentClipped(this.shoot, this.graphics.imageData);
                break;
            }

        printFPS(this.graphics, getDeltaTime());
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }

    return this;
}