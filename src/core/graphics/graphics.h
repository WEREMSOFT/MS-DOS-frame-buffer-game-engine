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

typedef struct ViewPort
{
	float aspectRatio;
	PointI viewPortSize;
	PointI viewPortPosition;
} ViewPort;

typedef struct
{
	unsigned int textureId;
	unsigned int VAO;
	unsigned int shaderProgram;
	ViewPort viewPort;
	PointI mousePosition;
	bool mouseRightDown;
	GLFWwindow *window;
	ImageData imageData;
} Graphics;

Graphics graphicsCreate(int width, int height, bool fullScreen);
void graphicsSwapBuffers(Graphics _this);
void graphicsDestroy(Graphics _this);
void graphicsUpdateMouseCoordinates(Graphics *_this);
void graphicsClear(ImageData _this);
void graphicsClearColor(ImageData _this, Color color);
Color graphicsGetPixel(ImageData _this, PointI point);
#endif