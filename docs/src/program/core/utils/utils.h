#ifndef __UTILS_H__
#define __UTILS_H__
#include <math.h>
#include <stdio.h>
#include "../graphics/graphics.h"
static inline PointF positionUpdateIntoCircularMovenent()
{
    PointF this = {0};
    this.x = sinf(glfwGetTime()) * 40.f - 40;
    this.y = cosf(glfwGetTime()) * 40.f - 40;
    return this;
}

static inline void printFPS(Graphics _this, double deltaTime)
{

#define FPS_HISTORY 10
    static double fpsHistory[FPS_HISTORY] = {0};
    static int counter = 1;
    counter++;
    counter %= FPS_HISTORY;
    fpsHistory[counter] = (1 / deltaTime);

    int sum = 0;
    for (int i = 0; i < FPS_HISTORY; i++)
    {
        sum += fpsHistory[i];
    }

    float avg = sum / FPS_HISTORY;

    {
        char text[1000] = {0};
        snprintf(text, 1000, "fps: %d", (int)floor(avg));
        graphicsPrintString(_this.imageData, (PointI){230, 220}, text, (Color){0, 0xff, 0xff});
    }

    {
        char text[1000] = {0};
        snprintf(text, 1000, "ftime:%.4f", deltaTime);
        graphicsPrintString(_this.imageData, (PointI){230, 230}, text, (Color){0, 0xff, 0xff});
    }
}
// The first time is called the deltatime value will be wrong.
// Call it at least one time before using

#define initDeltaTime() getDeltaTime()

static inline double getDeltaTime()
{
    static double lastUpdate = 0;

    double deltaTime = glfwGetTime() - lastUpdate;

    // if the framerate is > 30 fps, probably the game is paused.
    deltaTime = deltaTime > 0.03 ? 0 : deltaTime;

    lastUpdate = glfwGetTime();

    return deltaTime;
}

#endif