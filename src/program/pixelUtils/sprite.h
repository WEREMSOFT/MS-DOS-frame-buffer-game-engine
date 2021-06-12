#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "graphics.h"

typedef struct
{
    Pointi position;
    Pointi size;
    Color *imageData;
} Sprite;

Sprite spriteCreate(char *file);
void spriteDraw(Sprite this, Graphics graphics);
void spriteDrawTransparent(Sprite this, Graphics graphics);

void spriteDestroy(Sprite this);
Sprite spriteCreateCkeckerBoard(Graphics graphics, int width, Color color1, Color color2);

#endif