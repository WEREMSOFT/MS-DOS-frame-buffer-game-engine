
#include "assetManager.h"

void spritesLoad(Sprite *this)
{
    this[ASSET_BACKGROUND] = spriteCreate("assets/320x240Test.png");
    this[ASSET_SIGHT] = spriteCreate("assets/aimcross.png");

    this[ASSET_SHOOT] = spriteCreate("assets/shoot.bmp");
    this[ASSET_SHOOT].animated = true;
    this[ASSET_SHOOT].animation.frameCount = 4;
    this[ASSET_SHOOT].animation.currentFrame = 0;
    this[ASSET_SHOOT].animation.frameWidth = 55;
    this[ASSET_SHOOT].animation.frameRate = 10.;

    this[ASSET_ENEMY_GREEN_BIG] = spriteCreate("assets/enemyGreenBig.bmp");
}
