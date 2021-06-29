#include "enemy.h"
#include <math.h>

Enemy enemyFlyingEggCreate(Sprite sprite)
{
    Enemy this = {0};
    this.type = ENEMY_TYPE_FLYING_EGG;
    this.sprite = sprite;
    this.sprite.position.y = 100;
    this.sprite.position.x = 500;
    return this;
}

Enemy enemyFlyingEggUpdateState(Enemy this, double deltaTime)
{
    static double phase = 0;
    phase += deltaTime * 5;
    this.sprite.position.y = sinf(phase) * 10 + 100;
    return this;
}