
#include "assetManager.h"

void spritesLoad(Sprite *this)
{
    this[ASSET_SHIP_BLUE] = spriteCreate("assets/Ships/Blue-1.png");
    this[ASSET_SHIP_RED] = spriteCreate("assets/Ships/Red-2.png");
    this[ASSET_SHIP_GREEN] = spriteCreate("assets/Ships/Green-1.png");
    this[ASSET_HERO_BULLET] = spriteCreate("assets/Projectiles/projectile03-4.png");
    this[ASSET_LOGO] = spriteCreate("assets/Decoration/mainMenuLogo01.png");
    this[ASSET_SHIP_THRUSTERS] = spriteCreate("assets/EngineThrusters/thrusters.png");
    this[ASSET_ENEMY_SHOOTING_EGG] = spriteCreate("assets/Ships/Enemies/Enemies-3.png");

    this[ASSET_SHIP_THRUSTERS].animated = true;
    this[ASSET_SHIP_THRUSTERS].animation.frameCount = 4;
    this[ASSET_SHIP_THRUSTERS].animation.currentFrame = 0;
    this[ASSET_SHIP_THRUSTERS].animation.frameWidth = 16;
    this[ASSET_SHIP_THRUSTERS].animation.inverseFramerrate = 1 / 30;
}
