#include "enemy.h"
#define _USE_MATH_DEFINES
#include <math.h>

static Enemy processStateIdle(Enemy this, double deltaTime);
static Enemy processStateMovingForward(Enemy this, double deltaTime);
static Enemy processStateMovingBackwards(Enemy this, double deltaTime);
static Enemy processStateMovingCircle(Enemy this, double deltaTime);
static Enemy passToStateMovingCircle(Enemy this);
static Enemy passToStateMovingBackwards(Enemy this);

Enemy enemyFlyingEggCreate(Sprite sprite)
{
    Enemy this = {0};
    this.type = ENEMY_TYPE_FLYING_EGG;
    this.state = ENEMY_STATE_MOVING_FORWARD;
    this.movementDef.elapsedTime = 0;
    this.sprite = sprite;
    this.movementDef.speed = 100;
    this.movementDef.radious = 50;
    this.movementDef.radialSpeed = 2.0;
    this.sprite.position.y = 100;
    this.sprite.position.x = 900;
    return this;
}

Enemy enemyFlyingEggUpdateState(Enemy this, double deltaTime)
{
    switch (this.state)
    {
    case ENEMY_STATE_IDLE:
        return processStateIdle(this, deltaTime);
    case ENEMY_STATE_MOVING_FORWARD:
        return processStateMovingForward(this, deltaTime);
    case ENEMY_STATE_MOVING_BACKWARDS:
        return processStateMovingBackwards(this, deltaTime);
    case ENEMY_STATE_MOVING_CIRCLE:
        return processStateMovingCircle(this, deltaTime);
    default:
        return this;
    }
}

static Enemy processStateIdle(Enemy this, double deltaTime)
{
    this.movementDef.elapsedTime += deltaTime;

    if (this.movementDef.elapsedTime > 3)
    {
        return passToStateMovingCircle(this);
    }

    this.movementDef.phase += deltaTime * 5;
    this.sprite.position.y = sinf(this.movementDef.phase) * 10 + 100;
    return this;
}

static Enemy processStateMovingForward(Enemy this, double deltaTime)
{
    this.movementDef.elapsedTime += deltaTime;

    if (this.sprite.position.x <= 400)
    {
        return passToStateMovingCircle(this);
    }

    this.movementDef.phase += deltaTime * 5;
    this.sprite.position.y = sinf(this.movementDef.phase) * 10 + 100;
    this.sprite.position.x -= this.movementDef.speed * deltaTime;
    return this;
}

static Enemy passToStateMovingBackwards(Enemy this)
{
    this.state = ENEMY_STATE_MOVING_BACKWARDS;
    return this;
}

static Enemy processStateMovingBackwards(Enemy this, double deltaTime)
{
    this.movementDef.elapsedTime += deltaTime;

    if (this.movementDef.elapsedTime > 5)
    {
        this.movementDef.elapsedTime = 0;
        this.state = ENEMY_STATE_MOVING_FORWARD;
        return this;
    }

    this.movementDef.phase += deltaTime * 5;
    this.sprite.position.y = sinf(this.movementDef.phase) * 10 + 100;
    this.sprite.position.x += this.movementDef.speed * deltaTime;

    return this;
}

static Enemy passToStateMovingCircle(Enemy this)
{
    this.state = ENEMY_STATE_MOVING_CIRCLE;
    this.movementDef.elapsedTime = 0;
    this.movementDef.phase = -M_PI_2;
    this.movementDef.center.x = this.sprite.position.x;
    this.movementDef.center.y = this.sprite.position.y + this.movementDef.radious;
    return this;
}

static Enemy processStateMovingCircle(Enemy this, double deltaTime)
{
    if (this.movementDef.phase >= M_PI * .75f)
    {
        return passToStateMovingBackwards(this);
    }
    this.movementDef.phase -= deltaTime * this.movementDef.radialSpeed;
    this.sprite.position.x = cosf(this.movementDef.phase) * this.movementDef.radious + this.movementDef.center.x;
    this.sprite.position.y = sinf(this.movementDef.phase) * this.movementDef.radious + this.movementDef.center.y;
    return this;
}