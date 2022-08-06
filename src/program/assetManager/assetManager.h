#ifndef __ASSET_MANAGER_H__
#define __ASSET_MANAGER_H__
#include "../core/sprite/sprite.h"
typedef enum
{
    ASSET_BACKGROUND,
    ASSET_SIGHT,
    ASSET_SHOOT,
    ASSET_ENEMY_GREEN_BIG,
    ASSET_ENEMY_GREEN_BIG_SHOOT,
    ASSET_ENEMY_GREEN_SMALL,
    ASSET_ENEMY_GREEN_SMALL_SHOOT,
    ASSET_COUNT
} Assets;

void spritesLoad(Sprite *this);

#endif
