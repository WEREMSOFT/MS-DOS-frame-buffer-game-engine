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

int x = 100;
int y = 100;

enum ShapeType
{
    CIRCLE,
    SQUARE,
    POINT,
    SHAPE_TYPE_COUNT
};

int shapeType = CIRCLE;
bool shouldClear = false;
void programMainLoop(Program this)
{
    double lastUpdate = glfwGetTime();
    double currentFPS = 0;
    Graphics graphics = this.graphics;

    Sprite ship = spriteCreate("assets/sprite.png");

    while (!glfwWindowShouldClose(this.graphics.window))
    {

        if (glfwGetKey(graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(graphics.window, true);
        }

        if (glfwGetKey(graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            y--;
        }

        if (glfwGetKey(graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            y++;
        }

        if (glfwGetKey(graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            x--;
        }

        if (glfwGetKey(graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            x++;
        }

        if (glfwGetKey(graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            shapeType = (shapeType + 1) % (SHAPE_TYPE_COUNT);
        }

        if (glfwGetKey(graphics.window, GLFW_KEY_C) == GLFW_PRESS)
        {
            shouldClear = !shouldClear;
        }

        x = fmax(0, fmin(x, graphics.textureWidth - 1));
        y = fmax(0, fmin(y, graphics.textureHeight - 1));

        if (shouldClear)
            textureDataClear(this.graphics);

        ship.x = x;
        ship.y = y;
        spriteDraw(ship, this.graphics);

        switch (shapeType)
        {
        case POINT:
            graphicsPutPixel(graphics, x, y, (Color){255, 0, 0});
            break;
        case SQUARE:
            graphicsDrawSquare(graphics, x, y, 50, 50, (Color){random() % 255, random() % 255, 0});
            break;
        case CIRCLE:
            graphicsDrawCircle(graphics, x, y, 50, (Color){random() % 255, random() % 255, 0});
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