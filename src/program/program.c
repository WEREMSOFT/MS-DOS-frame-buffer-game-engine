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

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods, Array *string)
{
    static Array *this = {0};

    if (this == NULL)
    {
        this = string;
    }

    if (action == GLFW_PRESS)
    {
        arrayInsertElement(&this, &key);
    }
}

Program programCreate()
{
    Program this = {0};
    this.graphics = graphicsCreate();
    this.string = arrayCreate(1000, sizeof(char));
    glfwSetKeyCallback(this.graphics.window, keyCallback);
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
        graphicsPrintString(this, (Pointi){100, 0}, text, (Color){0, 0xff, 0xff});
    }
    {
        graphicsDrawCircle(this, this.mousePosition, this.mouseRightDown ? 2 : 4, this.mouseRightDown ? (Color){255, 0, 0} : (Color){0, 255, 0});
        graphicsPutPixel(this, this.mousePosition, (Color){255, 255, 255});
    }
    lastUpdate = glfwGetTime();
}

void programMainLoop(Program this)
{
    Graphics graphics = this.graphics;
    keyCallback(NULL, 0, 0, 0, 0, this.string);
    while (!glfwWindowShouldClose(this.graphics.window))
    {
        graphicsUpdateMouseCoordinates(&graphics);
        graphicsClear(this.graphics);

        if (glfwGetKey(graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(graphics.window, true);
        }
        if (this.string->header.length > 0)
            graphicsPrintArray(graphics, (Pointi){0, 0}, this.string, (Color){0xff, 0xff, 0xff});

        printFPS(graphics);
        graphicsSwapBuffers(graphics);
        glfwPollEvents();
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
}