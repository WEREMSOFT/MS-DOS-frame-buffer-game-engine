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

    this.positions[QPOS_TOP_LEFT].x = 42;
    this.positions[QPOS_TOP_LEFT].y = 55;

    this.positions[QPOS_TOP].x = 125;
    this.positions[QPOS_TOP].y = 50;

    this.positions[QPOS_TOP_RIGHT].x = 260;
    this.positions[QPOS_TOP_RIGHT].y = 55;

    this.positions[QPOS_RIGHT].x = 265;
    this.positions[QPOS_RIGHT].y = 130;

    this.positions[QPOS_BOTTOM_RIGHT].x = 225;
    this.positions[QPOS_BOTTOM_RIGHT].y = 180;

    this.positions[QPOS_BOTTOM].x = 125;
    this.positions[QPOS_BOTTOM].y = 180;

    this.positions[QPOS_BOTTOM_LEFT].x = 60;
    this.positions[QPOS_BOTTOM_LEFT].y = 180;

    this.positions[QPOS_LEFT].x = 55;
    this.positions[QPOS_LEFT].y = 125;

    this.positions[QPOS_TOP_LEFT].x = 42;
    this.positions[QPOS_TOP_LEFT].y = 55;

    this.sound = sound;
    this.graphics = graphics;
    this.background = sprites[ASSET_BACKGROUND];
    this.enemyGreenBig = sprites[ASSET_ENEMY_GREEN_BIG];
    this.enemyGreenBig.position.y = this.graphics.imageData.size.y - this.enemyGreenBig.size.y;
    this.enemyGreenBig.position.x = 40;
    this.shoot = sprites[ASSET_SHOOT];
    this.sight = sprites[ASSET_SIGHT];
    this.shouldQuit = false;
    return this;
}

EnemyBig EnemyBigPassToStateIdle(EnemyBig this)
{
    this.state = ENEMY_STATE_IDLE;
}

MainMenu handleControls(MainMenu this)
{

    if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        this.sight.position = this.positions[QPOS_TOP_LEFT];
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.quadPosition = QPOS_TOP_LEFT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        this.sight.position = this.positions[QPOS_TOP_RIGHT];
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.quadPosition = QPOS_TOP_RIGHT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        this.sight.position = this.positions[QPOS_BOTTOM_LEFT];
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.quadPosition = QPOS_BOTTOM_LEFT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        this.sight.position = this.positions[QPOS_BOTTOM_RIGHT];
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.quadPosition = QPOS_BOTTOM_RIGHT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        this.sight.position = this.positions[QPOS_TOP];
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.quadPosition = QPOS_TOP;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        this.sight.position = this.positions[QPOS_BOTTOM];
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.quadPosition = QPOS_BOTTOM;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        this.sight.position = this.positions[QPOS_RIGHT];
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.quadPosition = QPOS_RIGHT;
        return this;
    }

    if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        this.sight.position = this.positions[QPOS_LEFT];
        spriteDrawTransparentClipped(this.sight, this.graphics.imageData);
        this.quadPosition = QPOS_LEFT;
        return this;
    }

    this.quadPosition = QPOS_NONE;
    return this;
}

MainMenu mainMenuUpdate(MainMenu this)
{
    soundPlaySpeech(this.sound, SPEECH_SELECT_SHIP);
    bool shouldContinue = true;

    while (shouldContinue && !this.shouldQuit)
    {
        float dt = getDeltaTime();
        graphicsClear(this.graphics.imageData);

        if (isKeyJustPressed(this.graphics.window, GLFW_KEY_ENTER))
        {
            shouldContinue = false;
            soundPlaySfx(this.sound, SFX_SELECT);
        }

        this.shouldQuit = isKeyJustPressed(this.graphics.window, GLFW_KEY_ESCAPE);
        spriteDrawClipped(this.background, this.graphics.imageData);
        spriteDrawTransparentClipped(this.enemyGreenBig, this.graphics.imageData);
        this = handleControls(this);

        if (this.shoot.animation.isPlaying)
        {
            spriteDrawTransparentAnimatedClipped(&this.shoot, this.graphics.imageData, dt);
        }
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {

            if (this.quadPosition != QPOS_NONE)
            {
                this.shoot.position = this.sight.position;
                this.shoot.position.x -= this.sight.size.x / 2;
                this.shoot.position.y -= this.sight.size.y / 2;
                spriteDrawTransparentAnimatedClipped(&this.shoot, this.graphics.imageData, dt);
                soundPlaySfx(this.sound, SFX_SHOOT_HERO);
            }
        }

        printFPS(this.graphics, dt);
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }

    return this;
}
