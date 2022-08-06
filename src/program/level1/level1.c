#include "level1.h"
#include <stdlib.h>
#include "../core/graphics/graphics.h"
#include "../utils/utils.h"
#include "../core/input/keyboard.h"
#include "../core/sprite/sprite.h"
#include "../assetManager/assetManager.h"

#define __ENEMY_H_IMPLEMENTATION__
#include "enemy.h"

static void setPositions(PointI *positions)
{
    positions[QPOS_TOP_LEFT].x = 42;
    positions[QPOS_TOP_LEFT].y = 55;

    positions[QPOS_TOP].x = 125;
    positions[QPOS_TOP].y = 50;

    positions[QPOS_TOP_RIGHT].x = 260;
    positions[QPOS_TOP_RIGHT].y = 55;

    positions[QPOS_RIGHT].x = 265;
    positions[QPOS_RIGHT].y = 130;

    positions[QPOS_BOTTOM_RIGHT].x = 225;
    positions[QPOS_BOTTOM_RIGHT].y = 180;

    positions[QPOS_BOTTOM].x = 125;
    positions[QPOS_BOTTOM].y = 180;

    positions[QPOS_BOTTOM_LEFT].x = 60;
    positions[QPOS_BOTTOM_LEFT].y = 180;

    positions[QPOS_LEFT].x = 55;
    positions[QPOS_LEFT].y = 125;

    positions[QPOS_TOP_LEFT].x = 42;
    positions[QPOS_TOP_LEFT].y = 55;
}

MainMenu handleControls(MainMenu _this)
{

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_TOP_LEFT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP_LEFT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_TOP_RIGHT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_BOTTOM_LEFT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM_LEFT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_BOTTOM_RIGHT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_TOP];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_BOTTOM];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_RIGHT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_LEFT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_LEFT;
        return _this;
    }

    _this.quadPosition = QPOS_NONE;
    return _this;
}

MainMenu mainMenuCreate(Graphics graphics, Sprite *sprites, Sound sound)
{
    MainMenu _this = {0};

    setPositions(_this.positions);

    _this.sound = sound;
    _this.graphics = graphics;
    _this.background = sprites[ASSET_BACKGROUND];
    _this.enemyGreenBig = sprites[ASSET_ENEMY_GREEN_BIG];
    _this.enemyGreenSmall = sprites[ASSET_ENEMY_GREEN_SMALL];

    ENEMY_OFFSET.y = _this.graphics.imageData.size.y - _this.enemyGreenBig.size.y - _this.positions[QPOS_BOTTOM_LEFT].y;
    ENEMY_OFFSET.x = 40 - _this.positions[QPOS_BOTTOM_LEFT].x;

    _this.shoot = sprites[ASSET_SHOOT];
    _this.sight = sprites[ASSET_SIGHT];
    _this.shouldQuit = false;

    for (int i = 0; i < 8; i++)
    {
        //_this.enemyBigs[i] = enemyBigPassToStateHidden(_this.enemyBigs[i]);
    }

    return _this;
}

static void enemiesDraw(MainMenu _this)
{
    for (int i = 0; i < 8; i++)
    {
        _this.enemyGreenBig.position = _this.positions[i];
        _this.enemyGreenBig.position.x += ENEMY_OFFSET.x;
        _this.enemyGreenBig.position.y += ENEMY_OFFSET.y;
        spriteDrawTransparentClipped(_this.enemyGreenBig, _this.graphics.imageData);
    }
}

MainMenu mainMenuUpdate(MainMenu _this)
{
    soundPlaySpeech(_this.sound, SPEECH_SELECT_SHIP);
    bool shouldContinue = true;
    double elapsedTime = glfwGetTime();
    while (shouldContinue && !_this.shouldQuit)
    {
        float dt = getDeltaTime();
        graphicsClear(_this.graphics.imageData);

        _this.shouldQuit = isKeyJustPressed(_this.graphics.window, GLFW_KEY_ESCAPE);
        spriteDrawClipped(_this.background, _this.graphics.imageData);

        if (glfwGetTime() - elapsedTime > 1.)
        {
            elapsedTime = glfwGetTime();
            int enemyToDisplay = 1 + rand() % 3;
            enemyBigPassToStateGoingUp(_this.enemyBigs[enemyToDisplay]);
        }

        _this = handleControls(_this);

        if (_this.shoot.animation.isPlaying)
        {
            spriteDrawTransparentAnimatedClipped(&_this.shoot, _this.graphics.imageData, dt);
        }
        else if (glfwGetKey(_this.graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {

            if (_this.quadPosition != QPOS_NONE)
            {
                _this.shoot.position = _this.sight.position;
                _this.shoot.position.x -= _this.sight.size.x / 2;
                _this.shoot.position.y -= _this.sight.size.y / 2;
                spriteDrawTransparentAnimatedClipped(&_this.shoot, _this.graphics.imageData, dt);
                soundPlaySfx(_this.sound, SFX_SHOOT_HERO);
            }
        }

        printFPS(_this.graphics, dt);
        graphicsSwapBuffers(_this.graphics);
        glfwPollEvents();
    }

    return _this;
}
