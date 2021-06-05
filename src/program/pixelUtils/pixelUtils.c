#include "pixelUtils.h"

inline void putPixel(Color *image, int x, int y, Color color)
{
    int position = (x + y * TEXTURE_WIDTH);
    image[position] = color;
}

void drawCircle(Color *image, double x, double y, double radious, Color color)
{
    for (int i = x - radious; i <= x + radious; i++)
    {
        for (int j = y - radious; j <= y + radious; j++)
        {
            if (floor(sqrt(pow(x - i, 2) + pow(y - j, 2))) == radious)
                putPixel(image, i, j, color);
        }
    }
}

inline void drawSquare(Color *image, double x, double y, double width, double height, Color color)
{
    for (int i = x; i <= x + width; i++)
    {
        for (int j = y; j <= y + height; j++)
        {
            if (j == y || j == y + height || i == x || i == x + width)
                putPixel(image, i, j, color);
        }
    }
}

inline void clear(Color *image)
{
    memset(image, 0, TEXTURE_WIDTH * TEXTURE_HEIGHT * sizeof(Color));
}