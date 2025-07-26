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

#define UR_RED     \
	(URColor)      \
	{              \
		0xFF, 0, 0 \
	}
#define UR_WHITE         \
	(URColor)            \
	{                    \
		0xFF, 0xFF, 0xFF \
	}
#define UR_BLUE    \
	(URColor)      \
	{              \
		0, 0, 0xFF \
	}
#define UR_GREEN   \
	(URColor)      \
	{              \
		0, 0xFF, 0 \
	}
#define UR_PURPLE     \
	(URColor)         \
	{                 \
		0xFF, 0, 0xFF \
	}
#define UR_YELLOW     \
	(URColor)         \
	{                 \
		0xFF, 0xFF, 0 \
	}
#define UR_BLACK \
	(URColor)    \
	{            \
		0, 0, 0  \
	}

typedef struct
{
	int x, y;
} URPointI;

typedef struct
{
	float x, y;
} URPointF;

typedef struct
{
	URPointI position;
	URPointI size;
} URRectI;

void urClearScreen(URColor clearColor)
{
	int x, y;
	for (x = 0; x < UR_SCREEN_WIDTH; x++)
		for (y = 0; y < UR_SCREEN_HEIGHT; y++)
		{
			URPointI p = (URPointI){x, y};
			UR_PUT_PIXEL(p.x, p.y, clearColor.r, clearColor.g, clearColor.b);
		}
}

void urDrawCircle(URPointI center, double radious, URColor color)
{
	int i, j;
	for (i = center.x - radious; i <= center.x + radious; i++)
	{
		for (j = center.y - radious; j <= center.y + radious; j++)
		{
			URPointI p = (URPointI){i, j};
			if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
				UR_PUT_PIXEL(p.x, p.y, color.r, color.g, color.b);
		}
	}
}

void urDrawCircleFill(URPointI center, double radious, URColor color)
{
	int i, j;
	for (i = center.x - radious; i <= center.x + radious; i++)
	{
		for (j = center.y - radious; j <= center.y + radious; j++)
		{
			URPointI p = (URPointI){i, j};
			if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) <= radious)
				UR_PUT_PIXEL(p.x, p.y, color.r, color.g, color.b);
		}
	}
}

void urDrawSquare(URPointI topLeftCorner, URPointI size, URColor color)
{
	int i, j;
	for (i = fmax(topLeftCorner.x, 0); i <= topLeftCorner.x + size.x; i++)
	{
		for (j = fmax(topLeftCorner.y, 0); j <= topLeftCorner.y + size.y; j++)
		{
			URPointI p = (URPointI){i, j};
			if (j == topLeftCorner.y || j == topLeftCorner.y + size.y || i == topLeftCorner.x || i == topLeftCorner.x + size.x)
				UR_PUT_PIXEL(p.x, p.y, color.r, color.g, color.b);
		}
	}
}

void urDrawSquareFill(URPointI topLeftCorner, URPointI size, URColor color)
{
	int i, j;
	for (i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
	{
		for (j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
		{
			URPointI p = (URPointI){i, j};
			if (j >= topLeftCorner.y || j <= topLeftCorner.y + size.y || i >= topLeftCorner.x || i <= topLeftCorner.x + size.x)
				UR_PUT_PIXEL(p.x, p.y, color.r, color.g, color.b);
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
		UR_PUT_PIXEL(pointA.x, pointA.y, color.r, color.g, color.b);

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
#define UR_FONT_COUNT 95

unsigned char urFonts[UR_FONT_COUNT][5] = {
    /* 0 */ {28, 50, 42, 38, 28},
    /* 1 */ {8, 24, 8, 8, 28},
    /* 2 */ {28, 34, 12, 16, 62},
    /* 3 */ {28, 34, 12, 34, 28},
    /* 4 */ {10, 18, 34, 62, 2},
    /* 5 */ {62, 32, 60, 2, 60},
    /* 6 */ {30, 32, 60, 34, 28},
    /* 7 */ {62, 2, 4, 8, 16},
    /* 8 */ {28, 34, 28, 34, 28},
    /* 9 */ {28, 34, 30, 2, 28},
    /* A */ {60, 36, 36, 60, 36},
    /* B */ {56, 36, 56, 36, 56},
    /* C */ {60, 36, 32, 36, 60},
    /* D */ {56, 36, 36, 36, 56},
    /* E */ {60, 32, 60, 32, 60},
    /* F */ {60, 32, 56, 32, 32},
    /* G */ {24, 32, 44, 36, 24},
    /* H */ {36, 36, 60, 36, 36},
    /* I */ {28, 8, 8, 8, 28},
    /* J */ {60, 4, 4, 36, 24},
    /* K */ {36, 40, 48, 40, 36},
    /* L */ {32, 32, 32, 32, 56},
    /* M */ {34, 54, 42, 34, 34},
    /* N */ {18, 26, 22, 18, 18},
    /* O */ {60, 36, 36, 36, 60},
    /* P */ {56, 36, 56, 32, 32},
    /* Q */ {60, 36, 36, 44, 60},
    /* R */ {56, 36, 56, 36, 36},
    /* S */ {28, 32, 24, 4, 56},
    /* T */ {56, 16, 16, 16, 16},
    /* U */ {36, 36, 36, 36, 24},
    /* V */ {36, 36, 36, 24, 16},
    /* W */ {34, 34, 42, 54, 34},
    /* X */ {36, 36, 24, 36, 36},
    /* Y */ {34, 34, 20, 8, 8},
    /* Z */ {62, 4, 8, 16, 62},
    /* a */ {0, 28, 4, 28, 28},
    /* b */ {32, 32, 56, 36, 56},
    /* c */ {0, 24, 32, 32, 24},
    /* d */ {4, 4, 28, 36, 28},
    /* e */ {0, 56, 56, 32, 56},
    /* f */ {24, 32, 48, 32, 32},
    /* g */ {24, 36, 28, 4, 24},
    /* h */ {32, 32, 56, 36, 36},
    /* i */ {8, 0, 8, 8, 8},
    /* j */ {4, 0, 4, 4, 24},
    /* k */ {32, 36, 40, 48, 36},
    /* l */ {32, 32, 32, 32, 24},
    /* m */ {0, 52, 42, 42, 42},
    /* n */ {0, 48, 40, 40, 40},
    /* o */ {0, 24, 36, 36, 24},
    /* p */ {0, 56, 40, 48, 32},
    /* q */ {0, 24, 36, 44, 28},
    /* r */ {0, 40, 48, 32, 32},
    /* s */ {0, 24, 48, 8, 56},
    /* t */ {16, 56, 16, 16, 8},
    /* u */ {0, 36, 36, 36, 24},
    /* v */ {0, 36, 36, 24, 16},
    /* w */ {0, 34, 42, 42, 54},
    /* x */ {0, 36, 24, 24, 36},
    /* y */ {0, 20, 20, 8, 8},
    /* z */ {0, 60, 8, 16, 60},
    /* % */ {49, 50, 12, 19, 35},
    /* . */ {0, 0, 0, 12, 12},
    /* # */ {20, 62, 20, 62, 20},
    /* < */ {6, 24, 32, 24, 6},
    /* > */ {24, 6, 1, 6, 24},
    /* , */ {0, 0, 0, 24, 16},
    /* ( */ {12, 16, 32, 16, 12},
    /* ) */ {24, 4, 2, 4, 24},
    /* { */ {12, 8, 48, 8, 12},
    /* } */ {24, 8, 4, 8, 24},
    /* * */ {42, 28, 62, 28, 42},
    /* " */ {20, 20, 0, 0, 0},
    /* ; */ {0, 24, 0, 24, 16}
};

void urDrawCharacter(URPointI topLeftCorner, unsigned int letter, URColor color)
{
	int i, j;
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j <= 8; j++)
		{
			if (urFonts[letter][i] & (0b1000000 >> j))
				UR_PUT_PIXEL(topLeftCorner.x + j, topLeftCorner.y + i, color.r, color.g, color.b);
		}
	}
}

void urDrawCharacterWithShadow(URPointI topLeftCorner, unsigned int letter, URColor color, URColor shadowColor)
{
	URPointI shadowPosition = {.x = topLeftCorner.x + 1, .y = topLeftCorner.y + 1};
	urDrawCharacter(shadowPosition, letter, shadowColor);
	urDrawCharacter(topLeftCorner, letter, color);
}

void urPrintString(URPointI topLeftCorner, char *string, URColor color)
{
	size_t stringLen = strlen(string);

	URPointI original_position = topLeftCorner;

	URPointI current_position = topLeftCorner;

	size_t i;
	for (i = 0; i < stringLen; i++)
	{
		if (string[i] == '\t')
		{
			current_position.x += 6 * 8;
		} else if (string[i] == '\n')
		{
			current_position.x = original_position.x;
			current_position.y += 6;
		} else {
			current_position.x += 6;
		}

		if (string[i] == '.')
		{
			int charOffset = 63;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == '%')
		{
			int charOffset = 62;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}
		
		if (string[i] == '#')
		{
			int charOffset = 64;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == '<')
		{
			int charOffset = 65;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == '>')
		{
			int charOffset = 66;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == ',')
		{
			int charOffset = 67;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == '(' )
		{
			int charOffset = 68;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == ')' )
		{
			int charOffset = 69;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == '{' )
		{
			int charOffset = 70;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == '}' )
		{
			int charOffset = 71;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == '*' )
		{
			int charOffset = 72;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == '"' )
		{
			int charOffset = 73;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] == ';' )
		{
			int charOffset = 74;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] >= '0' && string[i] <= '9')
		{
			urDrawCharacter(current_position, string[i] - '0', color);
			continue;
		}

		if (string[i] >= 'a' && string[i] <= 'z')
		{
			int charOffset = string[i] - 'a' + 36;
			urDrawCharacter(current_position, charOffset, color);
			continue;
		}

		if (string[i] >= 'A' && string[i] <= 'Z')
		{
			int charOffset = string[i] + 32 - 'a' + 10;
			urDrawCharacter(current_position, charOffset, color);
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
			target[w * pixSize] = lineData[w * pixSize + 2];	 /* Red */
			target[w * pixSize + 1] = lineData[w * pixSize + 1]; /* Greeb */
			target[w * pixSize + 2] = lineData[w * pixSize];	 /* Blue */
			if (pixSize == 4)
				target[w * pixSize + 3] = lineData[w * pixSize + 3]; /* Alpha */
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

URPointI urPointF2PointI(URPointF point)
{
	return (URPointI){point.x, point.y};
}

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
	int i, j;
	for(i = 0; i < _this.size.x; i++)
	{
		for(j = 0; j < _this.size.y; j++)
		{
			URColor color = _this.imageData[j * _this.size.x + i];
			UR_PUT_PIXEL(_this.position.x + i, _this.position.y + j, color.r, color.g, color.b);
		}
	}
}

void urSpriteDrawClipped(URSprite _this)
{
	int i, j;
	if (_this.isFlipped)
	{
		URPointI adjustedPosition = {_this.position.x - _this.center.x, _this.position.y + _this.center.y};

		int clippedWidth = fmin(adjustedPosition.x, _this.size.x);
		int clippedHeight = fmin(_this.size.y,
								 fmax(0, _this.size.y - (_this.size.y - UR_SCREEN_HEIGHT)));

		int clippedX = 0;
		int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;


		for(j = clippedY; j < clippedHeight; j++)
		{
			for(i = clippedX; i < clippedWidth; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i];
				UR_PUT_PIXEL(adjustedPosition.x - i, adjustedPosition.y + j, color.r, color.g, color.b);
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

		for(j = clippedY; j < clippedHeight; j++)
		{
			for(i = clippedX; i < clippedWidth; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i];
				UR_PUT_PIXEL(adjustedPosition.x + i, adjustedPosition.y + j, color.r, color.g, color.b);
			}
		}
	}
}

void urSpriteDrawTransparent(URSprite _this)
{
	int i, j;

	if (_this.isFlipped)
		for(j = 0; j < _this.size.y; j++)
		{
			for(i = 0; i < _this.size.x; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i];
				if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
					UR_PUT_PIXEL(_this.position.x + _this.size.x - i, _this.position.y + j, color.r, color.g, color.b);
			}
		}
	else
		for(j = 0; j < _this.size.y; j++)
		{
			for(i = 0; i < _this.size.x; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i];
				if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
					UR_PUT_PIXEL(_this.position.x + i, _this.position.y + j, color.r, color.g, color.b);
			}
		}
}

void urSpriteDrawTransparentClipped(URSprite _this)
{
	int i, j;

	if (_this.isFlipped)
	{
		URPointI adjustedPosition = {_this.position.x - _this.center.x, _this.position.y + _this.center.y};

		int clippedWidth = fmin(adjustedPosition.x, _this.size.x);
		int clippedHeight = fmin(_this.size.y,
								 fmax(0, _this.size.y - (_this.size.y - UR_SCREEN_HEIGHT)));

		int clippedX = 0;
		int clippedY = adjustedPosition.y < 0 ? -adjustedPosition.y : 0;

		for(j = clippedY; j < clippedHeight; j++)
		{
			for(i = clippedX; i < clippedWidth; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i];
				if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
					UR_PUT_PIXEL(adjustedPosition.x - i, adjustedPosition.y + j, color.r, color.g, color.b);
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

		for(j = clippedY; j < clippedHeight; j++)
		{
			for(i = clippedX; i < clippedWidth; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i];
				if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
					UR_PUT_PIXEL(adjustedPosition.x + i, adjustedPosition.y + j, color.r, color.g, color.b);
			}
		}
	}
}

void urSpriteDrawTransparentClippedLowerLine(URSprite _this, int lowerLineHeight)
{
	int i, j;

	int clippedWidth = fmin(_this.size.x,
							fmax(0, _this.size.x - (_this.size.x + _this.position.x - UR_SCREEN_WIDTH)));
	int clippedHeight = fmin(_this.size.y,
							 fmax(0, _this.size.y - (_this.size.y + _this.position.y - lowerLineHeight)));
	int clippedX = _this.position.x < 0 ? -_this.position.x : 0;
	int clippedY = _this.position.y < 0 ? -_this.position.y : 0;

	if (_this.isFlipped)
		for(j = clippedY; j < clippedHeight; j++)
		{
			for(i = clippedX; i < clippedWidth; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i];
				if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
					UR_PUT_PIXEL(_this.position.x + _this.size.x - i, _this.position.y + j, color.r, color.g, color.b);
			}
		}
	else
		for(j = clippedY; j < clippedHeight; j++)
		{
			for(i = clippedX; i < clippedWidth; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i];
				if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
					UR_PUT_PIXEL(_this.position.x + i, _this.position.y + j, color.r, color.g, color.b);
			}
		}
}

void urSpriteDestroy(URSprite _this)
{
	UR_FREE(_this.imageData);
}

URSprite urSpriteCreateCkeckerBoard(URPointI size, int checkerWidth, URColor color1, URColor color2)
{
	int x, y;

	URSprite _this = {0};
	URColor currentColor = color1;
	_this.imageData = UR_MALLOC(sizeof(URColor) * size.x * size.y);
	_this.size = size;
	for(y = 0; y < _this.size.y; y++)
	{
		for(x = 0; x < _this.size.x; x++)
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
	int i, j;

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

		for(j = clippedY; j < clippedHeight; j++)
		{
			for(i = clippedX; i < clippedWidth; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i +
												_this.animation.currentFrame * _this.animation.frameWidth];
				if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
					UR_PUT_PIXEL(adjustedPosition.x - i, adjustedPosition.y + j, color.r, color.g, color.b);
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

		for(j = clippedY; j < clippedHeight; j++)
		{
			for(i = clippedX; i < clippedWidth; i++)
			{
				URColor color = _this.imageData[j * _this.size.x + i +
												_this.animation.currentFrame * _this.animation.frameWidth];
				if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
					UR_PUT_PIXEL(adjustedPosition.x + i, adjustedPosition.y + j, color.r, color.g, color.b);
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
	int i, j;
	static double fpsHistory[FPS_HISTORY] = {0};
	static int counter = 1;
	counter++;
	counter %= FPS_HISTORY;
	fpsHistory[counter] = (1 / deltaTime);

	int sum = 0;
	for(i = 0; i < FPS_HISTORY; i++)
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