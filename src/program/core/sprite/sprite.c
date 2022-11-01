#include "sprite.h"
#include <math.h>
#include "../stackAllocator/staticAlloc.h"

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
    if (_this.isFlipped)
    {
        PointI adjustedPosition = {_this.position.x - _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(adjustedPosition.x, _this.size.x);
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y - imageData.size.y)));

        int clippedX = 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                graphicsPutPixel(imageData, (PointI){adjustedPosition.x - i, adjustedPosition.y + j}, color);
            }
        }
    }
    else
    {
        PointI adjustedPosition = {_this.position.x + _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(_this.size.x,
                                fmax(0, _this.size.x - (_this.size.x + adjustedPosition.x - imageData.size.x)));
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + adjustedPosition.y - imageData.size.y)));

        int clippedX = adjustedPosition.x < 0 ? -adjustedPosition.x : 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                graphicsPutPixel(imageData, (PointI){adjustedPosition.x + i, adjustedPosition.y + j}, color);
            }
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
        PointI adjustedPosition = {_this.position.x - _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(adjustedPosition.x, _this.size.x);
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y - imageData.size.y)));

        int clippedX = 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){adjustedPosition.x - i, adjustedPosition.y + j}, color);
            }
        }
    }
    else
    {
        PointI adjustedPosition = {_this.position.x + _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(_this.size.x,
                                fmax(0, _this.size.x - (_this.size.x + adjustedPosition.x - imageData.size.x)));
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + adjustedPosition.y - imageData.size.y)));

        int clippedX = adjustedPosition.x < 0 ? -adjustedPosition.x : 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){adjustedPosition.x + i, adjustedPosition.y + j}, color);
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

Sprite spriteDrawTransparentAnimatedClipped(Sprite _this, ImageData imageData, double deltaTime)
{
    if (!_this.animation.isPlaying)
    {
        _this.animation.isPlaying = true;
    }

    _this.animation.frameIncrement += deltaTime * _this.animation.frameRate;
    int lastFrame = _this.animation.currentFrame;
    _this.animation.currentFrame = _this.animation.frameIncrement;
    _this.animation.currentFrame %= _this.animation.frameCount;
    if (_this.animation.currentFrame < lastFrame)
    {
        _this.animation.isPlaying = false;
    }

    if (_this.isFlipped)
    {
        PointI adjustedPosition = {_this.position.x - _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(_this.position.x + _this.size.y, _this.animation.frameWidth);

        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y - imageData.size.y)));

        int clippedX = 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i +
                                              _this.animation.currentFrame * _this.animation.frameWidth];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){adjustedPosition.x - i, adjustedPosition.y + j}, color);
            }
        }
        return _this;
    }

    {
        PointI adjustedPosition = {_this.position.x + _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(_this.animation.frameWidth,
                                fmax(0, _this.animation.frameWidth - (_this.animation.frameWidth + adjustedPosition.x -
                                                                      imageData.size.x)));

        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + adjustedPosition.y - imageData.size.y)));

        int clippedX = adjustedPosition.x < 0 ? -adjustedPosition.x : 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int i = clippedX; i < clippedWidth; i++)
        {
            for (int j = clippedY; j < clippedHeight; j++)
            {
                Color color = _this.imageData[j * _this.size.x + i +
                                              _this.animation.currentFrame * _this.animation.frameWidth];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    graphicsPutPixel(imageData, (PointI){adjustedPosition.x + i, adjustedPosition.y + j}, color);
            }
        }
        return _this;
    }
}
