#include "sprite.h"
#include <stb_image.h>
#include <math.h>

Sprite spriteCreate(char *file)
{
    Sprite this = {0};
    int nrChannels;

    this.imageData = (Color *)stbi_load(file, &this.size.x, &this.size.y, &nrChannels, 0);

    return this;
}

void spriteDraw(Sprite this, Graphics graphics)
{
    for (int i = 0; i < this.size.x; i++)
    {
        for (int j = 0; j < this.size.y; j++)
        {
            Color color = this.imageData[j * this.size.x + i];
            graphicsPutPixel(graphics, (PointI){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDrawClipped(Sprite this, Graphics graphics)
{
    int clippedWidth = fmin(this.size.x, fmax(0, this.size.x - (this.size.x + this.position.x - graphics.textureWidth)));
    int clippedHeight = fmin(this.size.y, fmax(0, this.size.y - (this.size.y + this.position.y - graphics.textureHeight)));

    int clippedX = this.position.x < 0 ? -this.position.x : 0;
    int clippedY = this.position.y < 0 ? -this.position.y : 0;

    for (int i = clippedX; i < clippedWidth; i++)
    {
        for (int j = clippedY; j < clippedHeight; j++)
        {
            Color color = this.imageData[j * this.size.x + i];
            graphicsPutPixel(graphics, (PointI){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDrawTransparent(Sprite this, Graphics graphics)
{
    for (int i = 0; i < this.size.x; i++)
    {
        for (int j = 0; j < this.size.y; j++)
        {
            Color color = this.imageData[j * this.size.x + i];
            if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                graphicsPutPixel(graphics, (PointI){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDrawTransparentClipped(Sprite this, Graphics graphics)
{
    int clippedWidth = fmin(this.size.x, fmax(0, this.size.x - (this.size.x + this.position.x - graphics.textureWidth)));
    int clippedHeight = fmin(this.size.y, fmax(0, this.size.y - (this.size.y + this.position.y - graphics.textureHeight)));

    int clippedX = this.position.x < 0 ? -this.position.x : 0;
    int clippedY = this.position.y < 0 ? -this.position.y : 0;

    for (int i = clippedX; i < clippedWidth; i++)
    {
        for (int j = clippedY; j < clippedHeight; j++)
        {
            Color color = this.imageData[j * this.size.x + i];
            if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                graphicsPutPixel(graphics, (PointI){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDestroy(Sprite this)
{
    free(this.imageData);
}

Sprite spriteCreateCkeckerBoard(Graphics graphics, int width, Color color1, Color color2)
{
    Sprite this = {0};

    Color currentColor = color1;
    this.imageData = malloc(sizeof(Color) * graphics.textureHeight * graphics.textureWidth);
    this.size = (PointI){graphics.textureWidth, graphics.textureHeight};
    for (int y = 0; y < graphics.textureHeight; y++)
    {
        for (int x = 0; x < graphics.textureWidth; x++)
        {
            if ((y / width + x / width) % 2)
            {
                currentColor = color1;
            }
            else
            {
                currentColor = color2;
            }
            this.imageData[x + y * graphics.textureWidth] = currentColor;
        }
    }

    return this;
}