#include "program.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "input/input.h"
#include "utils/utils.h"
#include "assetManager/assetManager.h"

Program programCreate(Graphics graphics, Sprite *sprites)
{
    Program this = {0};
    this.graphics = graphics;

    this.hero = sprites[ASSET_SHIP_BLUE];

    this.projectiles[PROJECTILE_HERO] = sprites[ASSET_HERO_BULLET];

    this.background = spriteCreateCkeckerBoard(this.graphics, (PointI){800, 600}, 20, (Color){0xFE, 0xF0, 0xA5}, (Color){0x74, 0x81, 0x9D});
    return this;
}

static inline PointF updatePositionBasedOnKeyboard(GLFWwindow *window, PointF this, double deltaTime, float speed)
{
    this.x += speed * deltaTime * (glfwGetKey(window, GLFW_KEY_RIGHT) - glfwGetKey(window, GLFW_KEY_LEFT));
    this.y += speed * deltaTime * (glfwGetKey(window, GLFW_KEY_DOWN) - glfwGetKey(window, GLFW_KEY_UP));
    return this;
}

static inline void updateHeroProjectiles(Program *this, double deltaTime)
{
    for (int i = 0; i < MAX_HERO_BULLETS_ON_SCREEN; i++)
    {
        this->heroBulletsPositions[i].x += 700.0 * deltaTime;
    }
}

static inline void drawHeroProjectiles(Program this, Graphics graphics)
{
    for (int i = 0; i < MAX_HERO_BULLETS_ON_SCREEN; i++)
    {
        this.projectiles[PROJECTILE_HERO].position = this.heroBulletsPositions[i];
        spriteDrawTransparentClipped(this.projectiles[PROJECTILE_HERO], graphics);
    }
}

void programMainLoop(Program this)
{
    Graphics graphics = this.graphics;

    double deltaTime = 0;

    int bulletIndex = 0;

    while (!glfwWindowShouldClose(this.graphics.window))
    {
        deltaTime = getDeltaTime();
        graphicsUpdateMouseCoordinates(&graphics);
        graphicsClear(this.graphics);

        if (isKeyJustPressed(graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(graphics.window, true);
        }

        this.background.position = positionUpdateIntoCircularMovenent();

        this.hero.position = updatePositionBasedOnKeyboard(graphics.window, this.hero.position, deltaTime, 100.0);

        spriteDrawClipped(this.background, graphics);
        spriteDrawTransparentClipped(this.hero, graphics);
        if (isKeyJustPressed(graphics.window, GLFW_KEY_SPACE))
        {
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
}

void programDestroy(Program this)
{
}
