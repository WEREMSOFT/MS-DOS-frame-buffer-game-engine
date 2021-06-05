#include "sprite.h"
#include <stb_image.h>

Sprite spriteCreate(char *file)
{
    Sprite this = {0};
    int nrChannels;

    this.imageData = (Color *)stbi_load(file, &this.width, &this.height, &nrChannels, 0);

    return this;
}

void spriteDraw(Sprite this, Graphics graphics)
{
    for (int i = 0; i <= this.width; i++)
    {
        for (int j = 0; j < this.height; j++)
        {
            Color color = this.imageData[j * this.width + i];
            if (color.r != 0xFF && color.b != 0xFF && color.g != 0)
                graphicsPutPixel(graphics, this.x + i, this.y + j, color);
        }
    }
}