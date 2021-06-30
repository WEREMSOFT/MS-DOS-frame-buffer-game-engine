#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "../core/sprite/sprite.h"

typedef enum
{
    ENEMY_STATE_IDLE,
    ENEMY_STATE_MOVING_FORWARD,
    ENEMY_STATE_MOVING_CIRCLE,
    ENEMY_STATE_SHOOTING,
    ENEMY_STATE_MOVING_BACKWARDS,
    ENEMY_STATES_COUNT
} EnemyStates;

typedef enum
{
    ENEMY_TYPE_FLYING_EGG,
    ENEMY_TYPES_COUNT
} EnemyTypes;

typedef struct
{
    double phase;
    double elapsedTime;
    double speed;
    double radialSpeed;
    double radious;
    PointF center;
} MovementDefinition;

typedef struct
{
    EnemyTypes type;
    EnemyStates state;
    Sprite sprite;
    MovementDefinition movementDef;
} Enemy;

Enemy enemyFlyingEggCreate(Sprite sprite);
Enemy enemyFlyingEggUpdateState(Enemy this, double deltaTime);

#endif