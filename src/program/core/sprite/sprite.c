#include "sprite.h"
#include <math.h>
#include "../stackAllocator/staticAlloc.h"
#include <stb_image.h>

Sprite spriteCreate(char *file)
{
    Sprite _this = {0};
    int nrChannels;
    _this.imageData = (Color *)stbi_load(file, &_this.size.x, &_this.size.y, &nrChannels, 0);
    if (_this.imageData == NULL)
    {
        printf("Error loading image %s: %s\n", file, stbi_failure_reason());
        exit(-1);
    }
    return _this;
}

void spriteDraw(Sprite _this, ImageData imageData)
{
    for (int i = 0; i < _this.size.x; i++)
    {
        for (int j = 0; j < _this.size.y; j++)
        {
            Color color = _this.imageData[j * _this.size.x + i];
            graphicsPutPixel(imageData, (PointI){_this.position.x + i, _this.position.y + j}, color);
        }
    }
}

void spriteDrawClipped(Sprite _this, ImageData imageData)
{
    int clippedWidth = fmin(_this.size.x, fmax(0, _this.size.x - (_this.size.x + _this.position.x - imageData.size.x)));
    int clippedHeight = fmin(_this.size.y, fmax(0, _this.size.y - (_this.size.y + _this.position.y - imageData.size.y)));
    int clippedX = _this.position.x < 0 ? -_this.position.x : 0;
    int clippedY = _this.position.y < 0 ? -_this.position.y : 0;
    for (int i = clippedX; i < clippedWidth; i++)
    {
        for (int j = clippedY; j < clippedHeight; j++)
        {
            Color color = _this.imageData[j * _this.size.x + i];
            graphicsPutPixel(imageData, (PointI){_this.position.x + i, _this.position.y + j}, color);
        }
    }
}

void spriteDrawTransparent(Sprite _this, ImageData imageData)
{
    if (_this.isFlipped)
        for (int i = 0; i < _this.size.x; i++)
        {
            for (int j = 0; j < _this.size.y; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){_this.position.x + _this.size.x - i, _this.position.y + j}, color);
            }
        }
    else
        for (int i = 0; i < _this.size.x; i++)
        {
            for (int j = 0; j < _this.size.y; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){_this.position.x + i, _this.position.y + j}, color);
            }
        }
}

void spriteDrawTransparentClipped(Sprite _this, ImageData imageData)
{
    if (_this.isFlipped)
    {
        int clippedWidth = fmin(_this.position.x - _this.center.x, _this.size.x);
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y - imageData.size.y)));

        int clippedX = 0;
        int clippedY = _this.position.y < 0 ? -_this.position.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){_this.position.x - _this.center.x - i, _this.position.y + _this.center.y + j}, color);
                // graphicsPutPixel(imageData, (PointI){_this.position.x + _this.size.x - i, _this.position.y - 1 + j}, color);
            }
        }
    }
    else
    {
        int clippedWidth = fmin(_this.size.x,
                                fmax(0, _this.size.x - (_this.size.x + _this.position.x - imageData.size.x + _this.center.x)));
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + _this.position.y - imageData.size.y + _this.center.y)));

        int clippedX = _this.position.x < 0 ? -_this.position.x : 0;
        int clippedY = _this.position.y < 0 ? -_this.position.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){_this.position.x + _this.center.x + i, _this.position.y + _this.center.y + j}, color);
            }
        }
    }
}

void spriteDrawTransparentClippedLowerLine(Sprite _this, ImageData imageData, int lowerLineHeight)
{
    int clippedWidth = fmin(_this.size.x,
                            fmax(0, _this.size.x - (_this.size.x + _this.position.x - imageData.size.x)));
    int clippedHeight = fmin(_this.size.y,
                             fmax(0, _this.size.y - (_this.size.y + _this.position.y - lowerLineHeight)));
    int clippedX = _this.position.x < 0 ? -_this.position.x : 0;
    int clippedY = _this.position.y < 0 ? -_this.position.y : 0;

    if (_this.isFlipped)
        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){_this.position.x + _this.size.x - i, _this.position.y + j}, color);
            }
        }
    else
        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){_this.position.x + i, _this.position.y + j}, color);
            }
        }
}

void spriteDestroy(Sprite _this)
{
    freeStatic(_this.imageData);
}

Sprite spriteCreateCkeckerBoard(PointI size, int checkerWidth, Color color1, Color color2)
{
    Sprite _this = {0};
    Color currentColor = color1;
    _this.imageData = allocStatic(sizeof(Color) * size.x * size.y);
    _this.size = size;
    for (int y = 0; y < _this.size.y; y++)
    {
        for (int x = 0; x < _this.size.x; x++)
        {
            if ((y / checkerWidth + x / checkerWidth) % 2)
            {
                currentColor = color1;
            }
            else
            {
                currentColor = color2;
            }
            _this.imageData[x + y * size.x] = currentColor;
        }
    }
    return _this;
}

void spriteDrawTransparentAnimatedClipped(Sprite *thisP, ImageData imageData, double deltaTime)
{
    Sprite _this = *thisP;
    if (!thisP->animation.isPlaying)
    {
        thisP->animation.isPlaying = true;
    }

    thisP->animation.frameIncrement += deltaTime * thisP->animation.frameRate;
    int lastFrame = thisP->animation.currentFrame;
    thisP->animation.currentFrame = _this.animation.frameIncrement;
    thisP->animation.currentFrame %= _this.animation.frameCount;
    if (thisP->animation.currentFrame < lastFrame)
    {
        thisP->animation.isPlaying = false;
    }

    if (_this.isFlipped)
    {
        int clippedWidth = fmin(_this.position.x + _this.size.y, _this.animation.frameWidth);

        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + _this.position.y -
                                                         imageData.size.y)));
        int clippedX = 0;
        int clippedY = _this.position.y < 0 ? -_this.position.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i +
                                              _this.animation.currentFrame * _this.animation.frameWidth];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData,
                                     (PointI){_this.position.x + _this.size.x - i, _this.position.y + j},
                                     color);
            }
        }
    }
    else
    {
        int clippedWidth = fmin(_this.animation.frameWidth,
                                fmax(0, _this.animation.frameWidth - (_this.animation.frameWidth + _this.position.x -
                                                                      imageData.size.x)));
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + _this.position.y -
                                                         imageData.size.y)));
        int clippedX = _this.position.x < 0 ? -_this.position.x : 0;
        int clippedY = _this.position.y < 0 ? -_this.position.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i +
                                              _this.animation.currentFrame * _this.animation.frameWidth];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData,
                                     (PointI){_this.position.x + i, _this.position.y + j},
                                     color);
            }
        }
    }
}
