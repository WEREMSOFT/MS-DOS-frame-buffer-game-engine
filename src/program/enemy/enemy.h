#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "../core/sprite/sprite.h"

typedef enum
{
    ENEMY_STATE_IDLE,
    ENEMY_STATE_MOVING_LEFT,
    ENEMY_STATE_MOVING_RIGHT,
    ENEMY_STATE_MOVING_UP,
    ENEMY_STATE_MOVING_DOWN,
    ENEMY_STATE_SHOOTING,
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
    PointF position;
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