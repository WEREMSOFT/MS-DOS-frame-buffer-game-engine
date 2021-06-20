#include "level.h"
#include "../assetManager/assetManager.h"
#include "levelStaticFunctions.h"
#include "../utils/utils.h"
#include "../core/input/keyboard.h"

#include <stdlib.h>
#include <soloud_c.h>

Level levelCreate(Graphics graphics, Sprite *sprites, Sound sound)
{
    Level this = {0};
    this.graphics = graphics;
    this.sound = sound;
    this.hero = sprites[ASSET_SHIP_BLUE];
    this.projectiles[PROJECTILE_HERO] = sprites[ASSET_HERO_BULLET];
    this.shouldQuit = false;
    return this;
}

Level levelMainLoop(Level this)
{
    Graphics graphics = this.graphics;

    double deltaTime = 0;
    int bulletIndex = 0;

#define STAR_COUNT 1000
    Star stars[STAR_COUNT] = {0};

    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].x = (random() % (graphics.screenSize.x));
        stars[i].y = (random() % (graphics.screenSize.y));
        stars[i].z = (double)random() / (double)RAND_MAX;
    }
    float speed = -160.f;
    bool shouldExit = false;
    while (!(shouldExit || this.shouldQuit))
    {
        deltaTime = getDeltaTime();
        graphicsUpdateMouseCoordinates(&graphics);
        graphicsClear(this.graphics);

        if (isKeyJustPressed(graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            this.shouldQuit = true;
        }

        if (isKeyJustPressed(graphics.window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        {
            shouldExit = true;
        }

        for (int i = 0; i < STAR_COUNT; i++)
        {
            stars[i].x = stars[i].x + speed * deltaTime * stars[i].z;
            stars[i].x = fmod((fmod(stars[i].x, graphics.screenSize.x) + graphics.screenSize.x), graphics.screenSize.x);

            graphicsPutPixel(graphics, (PointI){stars[i].x, stars[i].y}, (Color){stars[i].z * 255, stars[i].z * 255, stars[i].z * 255});
        }

        this.hero.position = updatePositionBasedOnKeyboard(graphics.window, this.hero.position, deltaTime, 100.0);

        spriteDrawTransparentClipped(this.hero, graphics);
        if (isKeyJustPressed(graphics.window, GLFW_KEY_SPACE))
        {
            soundPlaySfx(this.sound, SFX_SHOOT_HERO);
            bulletIndex++;
            bulletIndex %= MAX_HERO_BULLETS_ON_SCREEN;
            this.heroBulletsPositions[bulletIndex].y = this.hero.position.y + this.hero.size.y * .5;
            this.heroBulletsPositions[bulletIndex].x = this.hero.position.x + this.projectiles[PROJECTILE_HERO].size.x * 0.5;
        }

        updateHeroProjectiles(&this, deltaTime);

        drawHeroProjectiles(this, graphics);

        printFPS(graphics, deltaTime);
        graphicsSwapBuffers(graphics);
        glfwPollEvents();
    }

    return this;
}
