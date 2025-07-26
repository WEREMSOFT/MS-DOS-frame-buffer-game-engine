#ifndef __UTILS_H__
#define __UTILS_H__
#include <math.h>
#include <stdio.h>
#include "../graphics/graphics.h"
static PointF positionUpdateIntoCircularMovenent()
{
    PointF this = {0};
    this.x = sinf(glfwGetTime()) * 40.f - 40;
    this.y = cosf(glfwGetTime()) * 40.f - 40;
    return this;
}
/* The first time is called the deltatime value will be wrong.
Call it at least one time before using*/

#define initDeltaTime() getDeltaTime()

static double getDeltaTime()
{
    static double lastUpdate = 0;

    double deltaTime = glfwGetTime() - lastUpdate;

    /* if the framerate is > 30 fps, probably the game is paused. */
    deltaTime = deltaTime > 0.03 ? 0 : deltaTime;

    lastUpdate = glfwGetTime();

    return deltaTime;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif