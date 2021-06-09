#include "program.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <math.h>
#include "pixelUtils/sprite.h"

Program programCreate()
{
    Program this = {0};
    this.graphics = graphicsCreate();
    return this;
}

typedef struct
{
    double x, y, z;
} Star;

void printFPS(Graphics this)
{
    static double lastUpdate;
    static double currentFPS;

    currentFPS = (currentFPS + 1 / (glfwGetTime() - lastUpdate)) / 2;
    {
        char text[1000] = {0};
        snprintf(text, 1000, "fps: %d", (int)floor(currentFPS));
        graphicsPrintString(this, 100, 0, text, (Color){0, 0xff, 0xff});
    }
    {
        graphicsUpdateMouseCoordinates(&this);
        graphicsDrawCircle(this, this.mouseX, this.mouseY, this.mouseRightDown ? 2 : 4, this.mouseRightDown ? (Color){255, 0, 0} : (Color){0, 255, 0});
        graphicsPutPixel(this, this.mouseX, this.mouseY, (Color){255, 255, 255});
    }
    lastUpdate = glfwGetTime();
}

#define STAR_COUNT 1000

void drawStars(Graphics graphics, Star *stars, double speed, double deltaTime)
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].x = stars[i].x + speed * deltaTime * stars[i].z;
        stars[i].x = fmod((fmod(stars[i].x, graphics.textureWidth) + graphics.textureWidth), graphics.textureWidth);

        graphicsPutPixel(graphics, stars[i].x, stars[i].y, (Color){stars[i].z * 255, stars[i].z * 255, stars[i].z * 255});
    }
}

void programMainLoop(Program this)
{

    Graphics graphics = this.graphics;

    Star *stars = (Star *)malloc(sizeof(Star) * STAR_COUNT);

    Sprite background = spriteCreate("assets/320x200.png");

    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].x = (random() % (graphics.textureWidth));
        stars[i].y = (random() % (graphics.textureHeight));
        stars[i].z = (double)random() / (double)RAND_MAX;
    }

    double lastTime = glfwGetTime();
    float speed = 100.f;

    while (!glfwWindowShouldClose(this.graphics.window))
    {
        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        graphicsClear(this.graphics);

        if (glfwGetKey(graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(graphics.window, true);
        }
        if (glfwGetKey(graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            speed = 100.f;
        }

        if (glfwGetKey(graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            speed = -100.f;
        }

        spriteDraw(background, graphics);

        drawStars(graphics, stars, speed, deltaTime);

        graphicsPrintFontTest(graphics);
        graphicsPrintString(graphics, graphics.mouseX, graphics.mouseY, "hello world 123", (Color){0xff, 0, 0});

        printFPS(graphics);

        graphicsSwapBuffers(graphics);
        glfwPollEvents();
    }
    free(stars);
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
}