#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "../program.h"

typedef struct
{
    int x, y, width, height;
    Color *imageData;
} Sprite;

Sprite spriteCreate(char *file);
void spriteDraw(Sprite this, Graphics graphics);
void spriteDrawTransparent(Sprite this, Graphics graphics);

#endif