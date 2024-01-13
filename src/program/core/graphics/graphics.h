#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include <GLFW/glfw3.h>
#include <stdbool.h>

#ifndef Color
typedef struct
{
	unsigned char r, g, b;
} Color;
#endif

#ifndef PointI
typedef struct
{
	int x;
	int y;
} PointI;
#endif

#ifndef PointF
typedef struct
{
	double x;
	double y;
} PointF;
#endif

typedef struct
{
	PointI size;
	int bufferSize;
	Color *data;
} ImageData;

typedef struct
{
	unsigned int textureId;
	unsigned int VAO;
	unsigned int shaderProgram;
	PointI mousePosition;
	bool mouseRightDown;
	GLFWwindow *window;
	ImageData imageData;
} Graphics;

Graphics graphicsCreate(int width, int height, bool fullScreen, int sizeMultiplier);
void graphicsSwapBuffers(Graphics _this);
void graphicsDestroy(Graphics _this);
void graphicsUpdateMouseCoordinates(Graphics *_this);
void graphicsClear(ImageData _this);
Color graphicsGetPixel(ImageData _this, PointI point);
#endif