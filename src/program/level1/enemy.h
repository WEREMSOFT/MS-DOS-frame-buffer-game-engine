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

#define ENEMY_SPEED 50.
#define ENEMY_DOWN_OFFSET 30
#define ENEMY_UP_OFFSET 20

extern PointI ENEMY_OFFSET;

typedef struct
{
    int spriteId;
    PointI basePosition;
    PointF position;
    EnemyState state;
} Enemy;

Enemy enemyPassToStateHidden(Enemy this);
Enemy enemyPassToStateGoingUp(Enemy this);
Enemy enemyPassToStateGoingDown(Enemy this);
void enemyProcessStateGoingDown(Enemy *enemies, float deltaTime);
void enemyProcessStateIdle(Enemy *enemies);
void enemyProcessStateGoingUp(Enemy *enemies, float deltaTime);

#endif