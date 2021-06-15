#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "graphics.h"

typedef struct
{
    PointF position;
    PointI size;
    Color *imageData;
} Sprite;

Sprite spriteCreate(char *file);
void spriteDraw(Sprite this, Graphics graphics);
void spriteDrawClipped(Sprite this, Graphics graphics);
void spriteDrawTransparent(Sprite this, Graphics graphics);
void spriteDrawTransparentClipped(Sprite this, Graphics graphics);

void spriteDestroy(Sprite this);
Sprite spriteCreateCkeckerBoard(Graphics graphics, PointI size, int checkerWidth, Color color1, Color color2);

#endif