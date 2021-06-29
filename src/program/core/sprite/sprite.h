#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "../graphics/graphics.h"

typedef struct
{
    int frameWidth;
    int t;
    int currentFrame;
    int frameCount;
    float frameIncrement;
    float inverseFramerrate;
} Animation;

typedef struct
{
    bool animated;
    Animation animation;
    PointF position;
    PointI size;
    Color *imageData;
} Sprite;

Sprite spriteCreate(char *file);
void spriteDraw(Sprite this, Graphics graphics);
void spriteDrawClipped(Sprite this, Graphics graphics);
void spriteDrawTransparent(Sprite this, Graphics graphics);
void spriteDrawTransparentClipped(Sprite this, Graphics graphics);
void spriteDrawTransparentAnimatedClipped(Sprite *thisP, Graphics graphics, double deltaTime);
void spriteDestroy(Sprite this);
Sprite spriteCreateCkeckerBoard(PointI size, int checkerWidth, Color color1, Color color2);

#endif
