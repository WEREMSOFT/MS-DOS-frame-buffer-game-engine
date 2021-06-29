#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "../core/sprite/sprite.h"

typedef enum
{
    ENEMY_FEGG_STATE_IDLE,
    ENEMY_FEGG_STATE_MOVING_FORWARD,
    ENEMY_FEGG_STATE_SHOOTING,
    ENEMY_FEGG_STATE_MOVING_BACK,
    ENEMY_FEGG_STATES_COUNT
} EnemyFlyingEggStates;

typedef enum
{
    ENEMY_TYPE_FLYING_EGG,
    ENEMY_TYPES_COUNT
} EnemyTypes;

typedef struct
{
    EnemyTypes type;
    unsigned int State;
    Sprite sprite;
} Enemy;

Enemy enemyFlyingEggCreate(Sprite sprite);
Enemy enemyFlyingEggUpdateState(Enemy this, double deltaTime);

#endif