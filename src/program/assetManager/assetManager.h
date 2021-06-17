#ifndef __ASSET_MANAGER_H__
#define __ASSET_MANAGER_H__
#include "../pixelUtils/sprite.h"
enum Assets
{
    ASSET_SHIP_BLUE,
    ASSET_HERO_BULLET,
    ASSET_COUNT
};

void spritesInit(Sprite *this);

#endif

#ifdef __ASSET_MANAGER_IMPLEMENTATION__
#undef __ASSET_MANAGER_IMPLEMENTATION__

void spritesInit(Sprite *this)
{
    this[ASSET_SHIP_BLUE] = spriteCreate("assets/Ships/Blue-1.png");
    this[ASSET_HERO_BULLET] = spriteCreate("assets/Projectiles/projectile03-4.png");
}

#endif