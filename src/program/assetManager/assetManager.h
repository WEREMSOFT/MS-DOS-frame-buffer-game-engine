#ifndef __ASSET_MANAGER_H__
#define __ASSET_MANAGER_H__
#include "../core/sprite/sprite.h"
enum Assets
{
    ASSET_BACKGROUND,
    ASSET_SIGHT,
    ASSET_SHOOT,
    ASSET_ENEMY_GREEN_BIG,
    ASSET_COUNT
};

void spritesLoad(Sprite *this);

#endif
