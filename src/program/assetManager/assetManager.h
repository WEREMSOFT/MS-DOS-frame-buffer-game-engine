#ifndef __ASSET_MANAGER_H__
#define __ASSET_MANAGER_H__
#include "../core/sprite/sprite.h"
enum Assets
{
    ASSET_SHIP_GREEN,
    ASSET_SHIP_BLUE,
    ASSET_SHIP_RED,
    ASSET_HERO_BULLET,
    ASSET_COUNT
};

void spritesLoad(Sprite *this);

#endif
