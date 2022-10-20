#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "../graphics/graphics.h"

typedef struct
{
    bool isPlaying;
    int frameWidth;
    int t;
    int currentFrame;
    int frameCount;
    float frameIncrement;
    float frameRate;
} Animation;

typedef struct
{
    bool animated;
    bool isFlipped;
    Animation animation;
    PointI position;
    PointI center;
    PointI size;
    Color *imageData;
} Sprite;

Sprite spriteCreate(char *file);
void spriteDraw(Sprite this, ImageData imageData);
void spriteDrawClipped(Sprite this, ImageData imageData);
void spriteDrawTransparent(Sprite this, ImageData ImageData);
void spriteDrawTransparentClipped(Sprite this, ImageData imageData);
Sprite spriteDrawTransparentAnimatedClipped(Sprite _this, ImageData imageData, double deltaTime);
void spriteDestroy(Sprite this);
Sprite spriteCreateCkeckerBoard(PointI size, int checkerWidth, Color color1, Color color2);
void spriteDrawTransparentClippedLowerLine(Sprite this, ImageData imageData, int lowerLineHeight);

#endif
