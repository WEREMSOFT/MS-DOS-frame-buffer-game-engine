#include "enemy.h"
#define _USE_MATH_DEFINES
#include <math.h>

static Enemy processStateIdle(Enemy this, double deltaTime);
static Enemy processStateMovingLeft(Enemy this, double deltaTime);
static Enemy processStateMovingRight(Enemy this, double deltaTime);
static Enemy passToStateMovingRight(Enemy this);
static Enemy passToStateMovingLeft(Enemy this);

Enemy passToStateMovingLeft(Enemy this)
{
    this.state = ENEMY_STATE_MOVING_LEFT;
    return this;
}

Enemy enemyFlyingEggCreate(Sprite sprite)
{
    Enemy this = {0};
    this.type = ENEMY_TYPE_FLYING_EGG;
    this.state = ENEMY_STATE_MOVING_LEFT;
    this.movementDef.elapsedTime = 0;
    this.sprite = sprite;
    this.movementDef.speed = 100;
    this.movementDef.position.y = 100;
    this.movementDef.position.x = 900;
    return this;
}

Enemy enemyFlyingEggUpdateState(Enemy this, double deltaTime)
{
    switch (this.state)
    {
    case ENEMY_STATE_IDLE:
        return processStateIdle(this, deltaTime);
    case ENEMY_STATE_MOVING_LEFT:
        return processStateMovingLeft(this, deltaTime);
    case ENEMY_STATE_MOVING_RIGHT:
        return processStateMovingRight(this, deltaTime);
    default:
        return this;
    }
}

static Enemy processStateIdle(Enemy this, double deltaTime)
{
    this.movementDef.elapsedTime += deltaTime;

    if (this.movementDef.elapsedTime > 3)
    {
        return passToStateMovingLeft(this);
    }

    this.movementDef.phase += deltaTime * 5;
    this.movementDef.position.y = sinf(this.movementDef.phase) * 10 + 100;
    return this;
}

static Enemy processStateMovingLeft(Enemy this, double deltaTime)
{
    this.movementDef.elapsedTime += deltaTime;

    if (this.movementDef.position.x < -30)
    {
        this.movementDef.position.x = 900;
    }
    this.movementDef.phase += deltaTime * 5;
    this.movementDef.position.y = sinf(this.movementDef.phase) * 5 + this.movementDef.center.y;
    this.movementDef.position.x -= this.movementDef.speed * deltaTime;
    return this;
}

static Enemy passToStateMovingRight(Enemy this)
{
    this.state = ENEMY_STATE_MOVING_RIGHT;
    return this;
}

static Enemy processStateMovingRight(Enemy this, double deltaTime)
{
    this.movementDef.elapsedTime += deltaTime;

    if (this.movementDef.elapsedTime > 5)
    {
        this.movementDef.elapsedTime = 0;
        this.state = ENEMY_STATE_MOVING_LEFT;
        return this;
    }

    this.movementDef.phase += deltaTime * 5;
    this.movementDef.position.y = sinf(this.movementDef.phase) * 5 + 100;
    this.movementDef.position.x += this.movementDef.speed * deltaTime;

    return this;
}