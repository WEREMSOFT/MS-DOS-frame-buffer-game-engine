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

void programMainLoop(Program this)
{
    double lastUpdate = glfwGetTime();
    double currentFPS = 0;
    Graphics graphics = this.graphics;

#define STAR_COUNT 10000
    Star stars[STAR_COUNT] = {0};

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

        for (int i = 0; i < STAR_COUNT; i++)
        {
            stars[i].x = stars[i].x + speed * deltaTime * stars[i].z;
            stars[i].x = fmod((fmod(stars[i].x, graphics.textureWidth) + graphics.textureWidth), graphics.textureWidth);

            graphicsPutPixel(graphics, stars[i].x, stars[i].y, (Color){stars[i].z * 255, stars[i].z * 255, stars[i].z * 255});
        }

        graphicsSwapBuffers(graphics);
        glfwPollEvents();
        currentFPS = 1 / (glfwGetTime() - lastUpdate);
        char windowTitle[1000] = {0};
        snprintf(windowTitle, 1000, "\r fps: %f", floor(currentFPS));
        glfwSetWindowTitle(graphics.window, windowTitle);
        lastUpdate = glfwGetTime();
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
}