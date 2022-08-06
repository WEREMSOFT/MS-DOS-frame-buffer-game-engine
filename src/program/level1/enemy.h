#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "../core/graphics/graphics.h"
#include <stdlib.h>

typedef enum
{
    ENEMY_STATE_HIDDEN,
    ENEMY_STATE_GOING_UP,
    ENEMY_STATE_GOING_DOWN,
    ENEMY_STATE_DEAD,
    ENEMY_STATE_COUNT
} EnemyState;

extern PointI ENEMY_OFFSET;

typedef struct
{
    int spriteId;
    PointI basePosition;
    PointI position;
    EnemyState state;
} Enemy;

Enemy enemyBigPassToStateHidden(Enemy this);
Enemy enemyBigPassToStateGoingUp(Enemy this);
Enemy enemyBigPassToStateGoingDown(Enemy this);
void enemyProcessStateGoingDown(Enemy *enemies);
void enemyProcessStateIdle(Enemy *enemies);
void enemyProcessStateGoingUp(Enemy *enemies);

#endif