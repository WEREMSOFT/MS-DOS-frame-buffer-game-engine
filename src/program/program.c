#include "program.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <math.h>
#include "input/input.h"
#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "array/array.h"
Program programCreate()
{
    Program this = {0};
    this.graphics = graphicsCreate();

    this.hero = spriteCreate("assets/Ships/Blue-1.png");

    this.projectiles[PROJECTILE_HERO] = spriteCreate("assets/Projectiles/projectile03-4.png");

    this.background = spriteCreateCkeckerBoard(this.graphics, (PointI){800, 600}, 20, (Color){0xFE, 0xF0, 0xA5}, (Color){0x74, 0x81, 0x9D});
    return this;
}

static inline void printFPS(Graphics this, double deltaTime)
{
    // needed for rolling average
    static double currentFPS;

    currentFPS = (currentFPS + 1 / deltaTime) / 2;
    {
        char text[1000] = {0};
        snprintf(text, 1000, "fps: %d", (int)floor(currentFPS));
        graphicsPrintString(this, (PointI){100, 0}, text, (Color){0, 0xff, 0xff});
    }
}

static inline PointF positionUpdateIntoCircularMovenent()
{
    PointF this = {0};
    this.x = sinf(glfwGetTime()) * 40.f - 40;
    this.y = cosf(glfwGetTime()) * 40.f - 40;
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
    for (int i = 0; i < 10; i++)
    {
        this->heroBulletsPositions[i].x += 700.0 * deltaTime;
    }
}

static inline void drawHeroProjectiles(Program this, Graphics graphics)
{
    for (int i = 0; i < 10; i++)
    {
        this.projectiles[PROJECTILE_HERO].position = this.heroBulletsPositions[i];
        spriteDrawTransparentClipped(this.projectiles[PROJECTILE_HERO], graphics);
    }
}

void programMainLoop(Program this)
{
    Graphics graphics = this.graphics;

    double lastUpdate = glfwGetTime();
    double deltaTime = 0;

    int bulletIndex = 0;

    while (!glfwWindowShouldClose(this.graphics.window))
    {
        deltaTime = glfwGetTime() - lastUpdate;
        lastUpdate = glfwGetTime();

        graphicsUpdateMouseCoordinates(&graphics);
        graphicsClear(this.graphics);

        if (glfwGetKey(graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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
            bulletIndex %= 10;
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
    graphicsDestroy(this.graphics);
}