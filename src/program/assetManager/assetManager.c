
#include "assetManager.h"

void spritesLoad(Sprite *this)
{
    this[ASSET_SHIP_BLUE] = spriteCreate("assets/Ships/Blue-1.png");
    this[ASSET_SHIP_RED] = spriteCreate("assets/Ships/Red-2.png");
    this[ASSET_SHIP_GREEN] = spriteCreate("assets/Ships/Green-1.png");
    this[ASSET_HERO_BULLET] = spriteCreate("assets/Projectiles/projectile03-4.png");
    this[ASSET_LOGO] = spriteCreate("assets/Decoration/mainMenuLogo01.png");
}
