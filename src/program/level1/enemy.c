#include "../core/graphics/graphics.h"
#include "enemy.h"
#include <stdlib.h>

PointI ENEMY_OFFSET = {-10, -10};

Enemy enemyBigPassToStateHidden(Enemy this)
{
    this.state = ENEMY_STATE_HIDDEN;
}

Enemy enemyBigPassToStateGoingUp(Enemy this)
{
    this.state = ENEMY_STATE_GOING_UP;
}

Enemy enemyBigPassToStateGoingDown(Enemy this)
{
    this.state = ENEMY_STATE_GOING_DOWN;
}

static int sortByStateHiddenFirst(Enemy *a, Enemy *b)
{
    return a->state == ENEMY_STATE_HIDDEN ? 1 : 0;
}

static int sortByStateGoingUpFirst(Enemy *a, Enemy *b)
{
    return a->state == ENEMY_STATE_GOING_UP ? 1 : 0;
}

void enemyProcessStateGoingDown(Enemy *enemies)
{
    qsort(enemies, 8, sizeof(Enemy), sortByStateHiddenFirst);
    for (int i = 0; i < 8 || enemies[i].state != ENEMY_STATE_GOING_UP; i++)
    {
        enemies[i].position.y++;
        if (enemies[i].position.y > enemies[i].basePosition.y + 50)
        {
            enemies[i] = enemyBigPassToStateHidden(enemies[i]);
        }
    }
}

void enemyProcessStateIdle(Enemy *enemies)
{
    qsort(enemies, 8, sizeof(Enemy), sortByStateHiddenFirst);
}

void enemyProcessStateGoingUp(Enemy *enemies)
{
    qsort(enemies, 8, sizeof(Enemy), sortByStateHiddenFirst);
    for (int i = 0; i < 8 || enemies[i].state != ENEMY_STATE_GOING_UP; i++)
    {
        enemies[i].position.y--;
        if (enemies[i].position.y > enemies[i].basePosition.y - 50)
        {
            enemies[i] = enemyBigPassToStateGoingDown(enemies[i]);
        }
    }
}