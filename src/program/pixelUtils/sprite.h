#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "../program.h"

typedef struct
{
    Pointi position;
    Pointi size;
    Color *imageData;
} Sprite;

Sprite spriteCreate(char *file);
void spriteDraw(Sprite this, Graphics graphics);
void spriteDrawTransparent(Sprite this, Graphics graphics);

#endif