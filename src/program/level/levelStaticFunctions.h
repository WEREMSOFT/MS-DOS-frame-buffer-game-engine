#ifndef __LEVEL_STATIC_FUNCTIONS_H__
#define __LEVEL_STATIC_FUNCTIONS_H__
#include "../core/sprite/sprite.h"

#include "level.h"

static inline PointF updatePositionBasedOnKeyboard(GLFWwindow *window, PointF this, double deltaTime, float speed)
{
    this.x += speed * deltaTime * (glfwGetKey(window, GLFW_KEY_RIGHT) - glfwGetKey(window, GLFW_KEY_LEFT));
    this.y += speed * deltaTime * (glfwGetKey(window, GLFW_KEY_DOWN) - glfwGetKey(window, GLFW_KEY_UP));
    return this;
}

static inline void updateHeroProjectiles(Level *this, double deltaTime)
{
    for (int i = 0; i < MAX_HERO_BULLETS_ON_SCREEN; i++)
    {
        this->heroBulletsPositions[i].x += 700.0 * deltaTime;
    }
}

static inline void drawHeroProjectiles(Level this, Graphics graphics)
{
    for (int i = 0; i < MAX_HERO_BULLETS_ON_SCREEN; i++)
    {
        this.projectiles[PROJECTILE_HERO].position = this.heroBulletsPositions[i];
        spriteDrawTransparentClipped(this.projectiles[PROJECTILE_HERO], graphics);
    }
}

#endif