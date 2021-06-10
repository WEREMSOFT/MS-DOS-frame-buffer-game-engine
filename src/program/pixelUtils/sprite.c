#include "sprite.h"
#include <stb_image.h>

Sprite spriteCreate(char *file)
{
    Sprite this = {0};
    int nrChannels;

    this.imageData = (Color *)stbi_load(file, &this.size.x, &this.size.y, &nrChannels, 0);

    return this;
}

void spriteDraw(Sprite this, Graphics graphics)
{
    for (int i = 0; i <= this.size.x; i++)
    {
        for (int j = 0; j < this.size.y; j++)
        {
            Color color = this.imageData[j * this.size.x + i];
            graphicsPutPixel(graphics, (Pointi){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDrawTransparent(Sprite this, Graphics graphics)
{
    for (int i = 0; i <= this.size.x; i++)
    {
        for (int j = 0; j < this.size.y; j++)
        {
            Color color = this.imageData[j * this.size.x + i];
            if (color.r != 0xFF && color.b != 0xFF && color.g != 0)
                graphicsPutPixel(graphics, (Pointi){this.position.x + i, this.position.y + j}, color);
        }
    }
}
