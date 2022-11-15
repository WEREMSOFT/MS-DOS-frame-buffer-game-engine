#ifndef __UNIVERSAL_RENDERER_H__
#define __UNIVERSAL_RENDERER_H__
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef UR_SCREEN_WIDTH
#error UR_SCREEN_WITH not defined
#define UR_SCREEN_WIDTH 320
#endif

#ifndef UR_SCREEN_HEIGHT
#error UR_SCREEN_HEIGHT not defined
#define UR_SCREEN_HEIGHT 240
#endif

#ifndef UR_PUT_PIXEL
#error UR_PUT_PIXEL not defined
#endif

#ifndef UR_MALLOC
#define UR_MALLOC malloc
#endif

#ifndef UR_REALLOC
#define UR_REALLOC realloc
#endif

#ifndef UR_FREE
#define UR_FREE free
#endif

typedef struct
{
    unsigned char r, g, b;
} URColor;

#define UR_RED (URColor){0xFF, 0, 0}
#define UR_WHITE (URColor){0xFF, 0xFF, 0xFF}
#define UR_BLUE (URColor){0, 0, 0xFF}
#define UR_GREEN (URColor){0, 0xFF, 0}
#define UR_PURPLE (URColor){0xFF, 0, 0xFF}
#define UR_YELLOW (URColor){0xFF, 0xFF, 0}
#define UR_BLACK (URColor){0, 0, 0}

typedef struct
{
    int x, y;
} URPointI;

typedef struct
{
    URPointI position;
    URPointI size;
} URRectI;

// forward declaration
void UR_PUT_PIXEL(URPointI point, URColor color);

void urPutPixel(URPointI point, URColor color);

void urClearScreen(URColor clearColor)
{
    for (int x = 0; x < UR_SCREEN_WIDTH; x++)
        for (int y = 0; y < UR_SCREEN_HEIGHT; y++)
            UR_PUT_PIXEL((URPointI){x, y}, clearColor);
}

void urDrawCircle(URPointI center, double radious, URColor color)
{

    for (int i = center.x - radious; i <= center.x + radious; i++)
    {

        for (int j = center.y - radious; j <= center.y + radious; j++)
        {

            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                UR_PUT_PIXEL((URPointI){i, j}, color);
        }
    }
}

void urDrawCircleFill(URPointI center, double radious, URColor color)
{

    for (int i = center.x - radious; i <= center.x + radious; i++)
    {

        for (int j = center.y - radious; j <= center.y + radious; j++)
        {

            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) <= radious)
                UR_PUT_PIXEL((URPointI){i, j}, color);
        }
    }
}

void urDrawSquare(URPointI topLeftCorner, URPointI size, URColor color)
{
    for (int i = fmax(topLeftCorner.x, 0); i <= topLeftCorner.x + size.x; i++)
    {
        for (int j = fmax(topLeftCorner.y, 0); j <= topLeftCorner.y + size.y; j++)
        {
            if (j == topLeftCorner.y || j == topLeftCorner.y + size.y || i == topLeftCorner.x || i == topLeftCorner.x + size.x)
                UR_PUT_PIXEL((URPointI){i, j}, color);
        }
    }
}

void urDrawSquareFill(URPointI topLeftCorner, URPointI size, URColor color)
{

    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {

        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {

            if (j >= topLeftCorner.y || j <= topLeftCorner.y + size.y || i >= topLeftCorner.x || i <= topLeftCorner.x + size.x)
                UR_PUT_PIXEL((URPointI){i, j}, color);
        }
    }
}

void urDrawLine(URPointI pointA, URPointI pointB, URColor color)
{
    int dx = abs(pointB.x - pointA.x), sx = pointA.x < pointB.x ? 1 : -1;
    int dy = abs(pointB.y - pointA.y), sy = pointA.y < pointB.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        urPutPixel(pointA, color);

        if (pointA.x == pointB.x && pointA.y == pointB.y)
            break;
        e2 = err;

        if (e2 > -dx)
        {
            err -= dy;
            pointA.x += sx;
        }

        if (e2 < dy)
        {
            err += dx;
            pointA.y += sy;
        }
    }
}

unsigned char urFonts[][5] =
    {
        // 0
        {0b011100,
         0b110010,
         0b101010,
         0b100110,
         0b011100},
        // 1
        {0b001000,
         0b011000,
         0b001000,
         0b001000,
         0b011100},
        // 2
        {0b011100,
         0b100010,
         0b001100,
         0b010000,
         0b111110},
        // 3
        {0b011100,
         0b100010,
         0b001100,
         0b100010,
         0b011100},
        // 4
        {0b001010,
         0b010010,
         0b100010,
         0b111110,
         0b000010},
        // 5
        {0b111110,
         0b100000,
         0b111100,
         0b000010,
         0b111100},
        // 6
        {0b011110,
         0b100000,
         0b111100,
         0b100010,
         0b011100},
        // 7
        {0b111110,
         0b000010,
         0b000100,
         0b001000,
         0b010000},
        // 8
        {0b011100,
         0b100010,
         0b011100,
         0b100010,
         0b011100},
        // 9
        {0b011100,
         0b100010,
         0b011110,
         0b000010,
         0b011100},
        // A
        {0b111100,
         0b100100,
         0b100100,
         0b111100,
         0b100100},
        // B
        {0b111000,
         0b100100,
         0b111000,
         0b100100,
         0b111000},
        // C
        {0b111100,
         0b100100,
         0b100000,
         0b100100,
         0b111100},
        // D
        {0b111000,
         0b100100,
         0b100100,
         0b100100,
         0b111000},
        // E
        {0b111100,
         0b100000,
         0b111100,
         0b100000,
         0b111100},
        // F
        {0b111100,
         0b100000,
         0b111000,
         0b100000,
         0b100000},
        // G
        {0b011000,
         0b100000,
         0b101100,
         0b100100,
         0b011000},
        // H
        {0b100100,
         0b100100,
         0b111100,
         0b100100,
         0b100100},
        // I
        {0b011100,
         0b001000,
         0b001000,
         0b001000,
         0b011100},
        // J
        {0b111100,
         0b000100,
         0b000100,
         0b100100,
         0b011000},
        // K
        {0b100100,
         0b101000,
         0b110000,
         0b101000,
         0b100100},
        // L
        {0b100000,
         0b100000,
         0b100000,
         0b100000,
         0b111000},
        // M
        {0b100010,
         0b110110,
         0b101010,
         0b100010,
         0b100010},
        // N
        {0b010010,
         0b011010,
         0b010110,
         0b010010,
         0b010010},
        // O
        {0b111100,
         0b100100,
         0b100100,
         0b100100,
         0b111100},
        // P
        {0b111000,
         0b100100,
         0b111000,
         0b100000,
         0b100000},
        // Q
        {0b111100,
         0b100100,
         0b100100,
         0b101100,
         0b111100},
        // R
        {0b111000,
         0b100100,
         0b111000,
         0b100100,
         0b100100},
        // S
        {0b011100,
         0b100000,
         0b011000,
         0b000100,
         0b111000},
        // T
        {0b111000,
         0b010000,
         0b010000,
         0b010000,
         0b010000},
        // U
        {0b100100,
         0b100100,
         0b100100,
         0b100100,
         0b011000},
        // V
        {0b100100,
         0b100100,
         0b100100,
         0b011000,
         0b010000},
        // W
        {0b100010,
         0b100010,
         0b101010,
         0b110110,
         0b100010},
        // X
        {0b100100,
         0b100100,
         0b011000,
         0b100100,
         0b100100},
        // Y
        {0b100010,
         0b100010,
         0b010100,
         0b001000,
         0b001000},
        // Z
        {0b111110,
         0b000100,
         0b001000,
         0b010000,
         0b111110},
        // %
        {0b110001,
         0b110010,
         0b001100,
         0b010011,
         0b100011},
        // .
        {0b000000,
         0b000000,
         0b000000,
         0b001100,
         0b001100}};

void urDrawCharacter(URPointI topLeftCorner, unsigned int letter, URColor color)
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j <= 8; j++)
        {
            if (urFonts[letter][i] & (0b1000000 >> j))
                urPutPixel((URPointI){topLeftCorner.x + j, topLeftCorner.y + i}, color);
        }
    }
}

void urPrintString(URPointI topLeftCorner, char *string, URColor color)
{
    size_t stringLen = strlen(string);

    for (size_t i = 0; i < stringLen; i++)
    {
        if (string[i] == '.')
        {
            int charOffset = 'z' - 'a' + 12;
            urDrawCharacter((URPointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
            continue;
        }

        if (string[i] == '%')
        {
            int charOffset = 'z' - 'a' + 11;
            urDrawCharacter((URPointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
            continue;
        }

        if (string[i] >= '0' && string[i] <= '9')
        {
            urDrawCharacter((URPointI){topLeftCorner.x + i * 6, topLeftCorner.y}, string[i] - '0', color);
            continue;
        }

        if (string[i] >= 'a' && string[i] <= 'z')
        {
            int charOffset = string[i] - 'a' + 10;
            urDrawCharacter((URPointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
            continue;
        }
    }
}

#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>

typedef struct tagBITMAPFILEHEADER
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

#define BITMAPFILEHEADER_SIZE 14

typedef struct tagBITMAPINFOHEADER
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPixPerMeter;
    int32_t biYPixPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImporant;
} __attribute__((packed)) BITMAPINFOHEADER;

#define BITMAPINFOHEADER_SIZE 40

unsigned char *urBMPLoad(char *fileName, int *imageWidth, int *imageHeight)
{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    FILE *fp;
    int downToUp = 1;
    unsigned char *Data;
    unsigned int size;

    int w, h, width, height;
    int lineSize;
    unsigned char *lineData;

    if (sizeof(fileHeader) != BITMAPFILEHEADER_SIZE || sizeof(infoHeader) != BITMAPINFOHEADER_SIZE)
    {
        printf("16bit width alighment required. %ld\n", sizeof(fileHeader));
        exit(-1);
    }

    if ((fp = fopen(fileName, "rb")) == NULL)
    {
        printf("No such file. (%s)\n", fileName);
        exit(-1);
    }

    if (fread(&fileHeader, sizeof(fileHeader), 1, fp) != 1)
    {
        printf("Not BMP file (filesize under 14Bytes)\n");
        exit(-1);
    }

    if (fileHeader.bfType != htons(0x424d))
    {
        printf("%s: Not BMP file type (magc number = '%04x')\n", fileName, fileHeader.bfType);
        exit(-1);
    }

    if (fread(&infoHeader.biSize, sizeof(infoHeader.biSize), 1, fp) != 1)
    {
        printf("%s: No infoHeader field (too small filesize)\n", fileName);
        exit(-1);
    }

    if (infoHeader.biSize != 40)
    {
        printf("%s: Not Windows BMP file type (header size = %d)\n", fileName, infoHeader.biSize);
        exit(-1);
    }

    if (fread(&infoHeader.biWidth, sizeof(infoHeader) - sizeof(infoHeader.biSize), 1, fp) != 1)
    {
        printf("%s: Not enough infoHeader field (too small filesize)\n", fileName);
        exit(-1);
    }

    width = infoHeader.biWidth;
    height = infoHeader.biHeight;
    printf("width = %d, height = %d\n", infoHeader.biWidth, infoHeader.biHeight);
    if (height < 0)
    {
        downToUp = 0;
        height *= -1;
        printf("(up to down scanned data)\n");
    }

    if (infoHeader.biPlanes != 1)
    {
        printf("Number of plain is not 1 (%d)\n", infoHeader.biPlanes);
        exit(-1);
    }
    printf("planes = %d\n", infoHeader.biPlanes);

    if ((infoHeader.biBitCount != 24) && (infoHeader.biBitCount != 32))
    {
        printf("%s: only 24 or 32 bits per pixel format is acceptable (%d)\n", fileName, infoHeader.biBitCount);
        exit(-1);
    }
    printf("bits/pixel = %d\n", infoHeader.biBitCount);

    fseek(fp, fileHeader.bfOffBits, SEEK_SET);

    if (infoHeader.biBitCount != 24)
    {
        printf("only 24 bit RGB bmp images are supported\n");
        exit(-1);
    }

    int pixSize = infoHeader.biBitCount / 8;
    size = width * height * pixSize;
    Data = (unsigned char *)UR_MALLOC(size);
    if (Data == NULL)
    {
        printf("cannot alocate memory for the picture (%d bytes)\n", size);
        exit(-1);
    }
    printf("Data %p (%d bytes allocated)\n", Data, size);

    lineSize = width * pixSize;
    if ((lineSize % 4) != 0)
        lineSize = ((lineSize / 4) + 1) * 4;

    lineData = (unsigned char *)UR_MALLOC(lineSize);
    if (Data == NULL)
    {
        printf("cannot alocate memory for line buffer (%d bytes)\n", lineSize);
        exit(-1);
    }

    for (h = 0; h < height; h++)
    {
        if (fread(lineData, lineSize, 1, fp) != 1)
        {
            printf("cannot read data body\n");
            exit(-1);
        }
        unsigned char *target;
        if (downToUp)
        {
            target = Data + (height - 1 - h) * width * pixSize;
        }
        else
        {
            target = Data + h * width * pixSize;
        }

        for (w = 0; w < width; w++)
        {
            target[w * pixSize] = lineData[w * pixSize + 2];     // Red
            target[w * pixSize + 1] = lineData[w * pixSize + 1]; // Green
            target[w * pixSize + 2] = lineData[w * pixSize];     // Blue
            if (pixSize == 4)
                target[w * pixSize + 3] = lineData[w * pixSize + 3]; // Alpha
        }
    }

    UR_FREE(lineData);
    fclose(fp);

    *imageWidth = width;
    *imageHeight = height;
    return Data;
}

typedef struct
{
    bool isPlaying;
    int frameWidth;
    int t;
    int currentFrame;
    int frameCount;
    float frameIncrement;
    float frameRate;
} Animation;

typedef struct
{
    bool animated;
    bool isFlipped;
    Animation animation;
    URPointI position;
    URPointI center;
    URPointI size;
    URColor *imageData;
} URSprite;

URSprite urSpriteCreate(char *file)
{
    URSprite _this = {0};
    int nrChannels;

    _this.imageData = (URColor *)urBMPLoad(file, &_this.size.x, &_this.size.y);
    if (_this.imageData == NULL)
    {
        printf("Error loading image %s\n", file);
        exit(-1);
    }
    return _this;
}

void urSpriteDraw(URSprite _this)
{
    for (int i = 0; i < _this.size.x; i++)
    {
        for (int j = 0; j < _this.size.y; j++)
        {
            URColor color = _this.imageData[j * _this.size.x + i];
            urPutPixel((URPointI){_this.position.x + i, _this.position.y + j}, color);
        }
    }
}

void urSpriteDrawClipped(URSprite _this)
{
    if (_this.isFlipped)
    {
        URPointI adjustedPosition = {_this.position.x - _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(adjustedPosition.x, _this.size.x);
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y - UR_SCREEN_HEIGHT)));

        int clippedX = 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int j = clippedY; j < clippedHeight; j++)
        {
            for (int i = clippedX; i < clippedWidth; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i];
                urPutPixel((URPointI){adjustedPosition.x - i, adjustedPosition.y + j}, color);
            }
        }
    }
    else
    {
        URPointI adjustedPosition = {_this.position.x + _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(_this.size.x,
                                fmax(0, _this.size.x - (_this.size.x + adjustedPosition.x - UR_SCREEN_WIDTH)));
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + adjustedPosition.y - UR_SCREEN_HEIGHT)));

        int clippedX = adjustedPosition.x < 0 ? -adjustedPosition.x : 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int j = clippedY; j < clippedHeight; j++)
        {
            for (int i = clippedX; i < clippedWidth; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i];
                urPutPixel((URPointI){adjustedPosition.x + i, adjustedPosition.y + j}, color);
            }
        }
    }
}

void urSpriteDrawTransparent(URSprite _this)
{
    if (_this.isFlipped)
        for (int j = 0; j < _this.size.y; j++)
        {
            for (int i = 0; i < _this.size.x; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    urPutPixel((URPointI){_this.position.x + _this.size.x - i, _this.position.y + j}, color);
            }
        }
    else
        for (int j = 0; j < _this.size.y; j++)
        {
            for (int i = 0; i < _this.size.x; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    urPutPixel((URPointI){_this.position.x + i, _this.position.y + j}, color);
            }
        }
}

void urSpriteDrawTransparentClipped(URSprite _this)
{
    if (_this.isFlipped)
    {
        URPointI adjustedPosition = {_this.position.x - _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(adjustedPosition.x, _this.size.x);
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y - UR_SCREEN_HEIGHT)));

        int clippedX = 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int j = clippedY; j < clippedHeight; j++)
        {
            for (int i = clippedX; i < clippedWidth; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    urPutPixel((URPointI){adjustedPosition.x - i, adjustedPosition.y + j}, color);
            }
        }
    }
    else
    {
        URPointI adjustedPosition = {_this.position.x + _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(_this.size.x,
                                fmax(0, _this.size.x - (_this.size.x + adjustedPosition.x - UR_SCREEN_WIDTH)));
        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + adjustedPosition.y - UR_SCREEN_HEIGHT)));

        int clippedX = adjustedPosition.x < 0 ? -adjustedPosition.x : 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int j = clippedY; j < clippedHeight; j++)
        {
            for (int i = clippedX; i < clippedWidth; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    urPutPixel((URPointI){adjustedPosition.x + i, adjustedPosition.y + j}, color);
            }
        }
    }
}

void urSpriteDrawTransparentClippedLowerLine(URSprite _this, int lowerLineHeight)
{
    int clippedWidth = fmin(_this.size.x,
                            fmax(0, _this.size.x - (_this.size.x + _this.position.x - UR_SCREEN_WIDTH)));
    int clippedHeight = fmin(_this.size.y,
                             fmax(0, _this.size.y - (_this.size.y + _this.position.y - lowerLineHeight)));
    int clippedX = _this.position.x < 0 ? -_this.position.x : 0;
    int clippedY = _this.position.y < 0 ? -_this.position.y : 0;

    if (_this.isFlipped)
        for (int j = clippedY; j < clippedHeight; j++)
        {
            for (int i = clippedX; i < clippedWidth; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    urPutPixel((URPointI){_this.position.x + _this.size.x - i, _this.position.y + j}, color);
            }
        }
    else
        for (int j = clippedY; j < clippedHeight; j++)
        {
            for (int i = clippedX; i < clippedWidth; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    urPutPixel((URPointI){_this.position.x + i, _this.position.y + j}, color);
            }
        }
}

void urSpriteDestroy(URSprite _this)
{
    UR_FREE(_this.imageData);
}

URSprite urSpriteCreateCkeckerBoard(URPointI size, int checkerWidth, URColor color1, URColor color2)
{
    URSprite _this = {0};
    URColor currentColor = color1;
    _this.imageData = UR_MALLOC(sizeof(URColor) * size.x * size.y);
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

URSprite urSpriteDrawTransparentAnimatedClipped(URSprite _this, double deltaTime)
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
        URPointI adjustedPosition = {_this.position.x - _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(_this.position.x + _this.size.y, _this.animation.frameWidth);

        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y - UR_SCREEN_HEIGHT)));

        int clippedX = 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int j = clippedY; j < clippedHeight; j++)
        {
            for (int i = clippedX; i < clippedWidth; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i +
                                                _this.animation.currentFrame * _this.animation.frameWidth];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    urPutPixel((URPointI){adjustedPosition.x - i, adjustedPosition.y + j}, color);
            }
        }
        return _this;
    }

    {
        URPointI adjustedPosition = {_this.position.x + _this.center.x, _this.position.y + _this.center.y};

        int clippedWidth = fmin(_this.animation.frameWidth,
                                fmax(0, _this.animation.frameWidth - (_this.animation.frameWidth + adjustedPosition.x -
                                                                      UR_SCREEN_WIDTH)));

        int clippedHeight = fmin(_this.size.y,
                                 fmax(0, _this.size.y - (_this.size.y + adjustedPosition.y - UR_SCREEN_HEIGHT)));

        int clippedX = adjustedPosition.x < 0 ? -adjustedPosition.x : 0;
        int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

        for (int j = clippedY; j < clippedHeight; j++)
        {
            for (int i = clippedX; i < clippedWidth; i++)
            {
                URColor color = _this.imageData[j * _this.size.x + i +
                                                _this.animation.currentFrame * _this.animation.frameWidth];
                if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                    urPutPixel((URPointI){adjustedPosition.x + i, adjustedPosition.y + j}, color);
            }
        }
        return _this;
    }
}


bool urHitTestRectRect(URRectI rectangleA, URRectI rectangleB)
{
    int a[4], b[4];

    a[0] = rectangleA.position.x;
    a[1] = rectangleA.position.y;

    a[2] = a[0] + rectangleA.size.x;
    a[3] = a[1] + rectangleA.size.y;

    b[0] = rectangleB.position.x;
    b[1] = rectangleB.position.y;

    b[2] = b[0] + rectangleB.size.x;
    b[3] = b[1] + rectangleB.size.y;

    return !(a[1] > b[3] || b[1] > a[3]) && !(a[0] > b[2] || b[0] > a[2]);
}

bool urHitTestPointRect(URPointI point, URRectI rectangle)
{
    if (point.x >= rectangle.position.x &&
        point.x <= rectangle.position.x + rectangle.size.x &&
        point.y >= rectangle.position.y &&
        point.y <= rectangle.position.y + rectangle.size.y)
            return true;

    return false;
}


void urPrintFPS(double deltaTime)
{

#define FPS_HISTORY 10
    static double fpsHistory[FPS_HISTORY] = {0};
    static int counter = 1;
    counter++;
    counter %= FPS_HISTORY;
    fpsHistory[counter] = (1 / deltaTime);

    int sum = 0;
    for (int i = 0; i < FPS_HISTORY; i++)
    {
        sum += fpsHistory[i];
    }

    float avg = sum / FPS_HISTORY;

    {
        char text[1000] = {0};
        snprintf(text, 1000, "fps: %d", (int)floor(avg));
        urPrintString((URPointI){230, 220}, text, (URColor){0, 0xff, 0xff});
    }

    {
        char text[1000] = {0};
        snprintf(text, 1000, "ftime:%.4f", deltaTime);
        urPrintString((URPointI){230, 230}, text, (URColor){0, 0xff, 0xff});
    }
}

#endif