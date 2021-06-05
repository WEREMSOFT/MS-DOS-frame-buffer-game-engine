#ifndef __PIXEL_UTILS_H__
#define __PIXEL_UTILS_H__

#include "../program.h"

static void putPixel(Color *image, int x, int y, Color color);
static void drawCircle(Color *image, double x, double y, double radious, Color color);
static void drawSquare(Color *image, double x, double y, double width, double height, Color color);
static void clear(Color *image);

#endif