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

    this.background = spriteCreateCkeckerBoard(this.graphics, (PointI){800, 600}, 20, (Color){0xFE, 0xF0, 0xA5}, (Color){0x74, 0x81, 0x9D});
    return this;
}

void printFPS(Graphics this)
{
    static double lastUpdate;
    static double currentFPS;

    currentFPS = (currentFPS + 1 / (glfwGetTime() - lastUpdate)) / 2;
    {
        char text[1000] = {0};
        snprintf(text, 1000, "fps: %d", (int)floor(currentFPS));
        graphicsPrintString(this, (PointI){100, 0}, text, (Color){0, 0xff, 0xff});
    }
    // {
    //     graphicsDrawCircle(this, this.mousePosition, this.mouseRightDown ? 2 : 4, this.mouseRightDown ? (Color){255, 0, 0} : (Color){0, 255, 0});
    //     graphicsPutPixel(this, this.mousePosition, (Color){255, 255, 255});
    // }
    lastUpdate = glfwGetTime();
}

PointF positionUpdateIntoCircularMovenent()
{
    PointF this = {0};
    this.x = sinf(glfwGetTime()) * 40.f - 40;
    this.y = cosf(glfwGetTime()) * 40.f - 40;
    return this;
}

PointF updatePositionBasedOnKeyboard(GLFWwindow *window, PointF this, double deltaTime, float speed)
{
    this.x += speed * deltaTime * (glfwGetKey(window, GLFW_KEY_RIGHT) - glfwGetKey(window, GLFW_KEY_LEFT));
    this.y += speed * deltaTime * (glfwGetKey(window, GLFW_KEY_DOWN) - glfwGetKey(window, GLFW_KEY_UP));
    return this;
}

void programMainLoop(Program this)
{
    Graphics graphics = this.graphics;

    double lastUpdate = glfwGetTime();
    double deltaTime = 0;

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

        printFPS(graphics);
        graphicsSwapBuffers(graphics);
        glfwPollEvents();
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
}