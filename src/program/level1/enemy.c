#include "../core/graphics/graphics.h"
#include "enemy.h"
#include <stdlib.h>

PointI ENEMY_OFFSET = {-10, -10};

Enemy enemyPassToStateHidden(Enemy this)
{
    this.state = ENEMY_STATE_HIDDEN;
    this.position.y = this.basePosition.y + ENEMY_DOWN_OFFSET;
    return this;
}

Enemy enemyPassToStateGoingUp(Enemy this)
{
    this.state = ENEMY_STATE_GOING_UP;
    this.position.y = this.basePosition.y + ENEMY_DOWN_OFFSET;
    return this;
}

Enemy enemyPassToStateGoingDown(Enemy this)
{
    this.state = ENEMY_STATE_GOING_DOWN;
    return this;
}

static int sortByStateHiddenFirst(Enemy *a, Enemy *b)
{
    return a->state == ENEMY_STATE_HIDDEN ? 1 : 0;
}

static int sortByStateGoingUpFirst(Enemy *a, Enemy *b)
{
    return a->state == ENEMY_STATE_GOING_UP ? 1 : 0;
}

void enemyProcessStateGoingDown(Enemy *enemies, float deltaTime)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_DOWN)
            continue;
        if (enemies[i].position.y > enemies[i].basePosition.y + ENEMY_DOWN_OFFSET)
        {
            enemies[i] = enemyPassToStateHidden(enemies[i]);
            continue;
        }
        enemies[i].position.y += ENEMY_SPEED * deltaTime;
    }
}

void enemyProcessStateIdle(Enemy *enemies)
{
    qsort(enemies, 8, sizeof(Enemy), sortByStateHiddenFirst);
}

void enemyProcessStateGoingUp(Enemy *enemies, float deltaTime)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_UP)
            continue;
        if (enemies[i].position.y < enemies[i].basePosition.y - ENEMY_UP_OFFSET)
        {
            enemies[i] = enemyPassToStateGoingDown(enemies[i]);
            continue;
        }
        enemies[i].position.y -= ENEMY_SPEED * deltaTime;
    }
}