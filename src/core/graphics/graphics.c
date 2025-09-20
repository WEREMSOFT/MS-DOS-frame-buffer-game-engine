#include "../utils/memory.h"
#include "graphics.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "../shader/shader.h"

extern char fonts[][5];
static void textureCreate(Graphics *_this)
{
    glGenTextures(1, &_this->textureId);
    glBindTexture(GL_TEXTURE_2D, _this->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    _this->imageData.bufferSize = _this->imageData.size.x * _this->imageData.size.y * sizeof(Color);
    _this->imageData.data = MALLOC(_this->imageData.bufferSize);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _this->imageData.size.x, _this->imageData.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, _this->imageData.data);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    /* make sure the viewport matches the new window dimensions; note that width and
     height will be significantly larger than specified on retina displays.*/
    glViewport(0, 0, width, height);
}

Graphics graphicsCreate(int width, int height, bool fullScreen)
{
    Graphics _this = {0};
    _this.imageData.size.x = width;
    _this.imageData.size.y = height;
    glfwInit();
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    int screenWidth = mode->width;
    int screenHeight = mode->height;

    if (!fullScreen)
    {
        screenWidth = width * 2;
        screenHeight = height * 2;
        monitor = NULL;
    }

    _this.window = glfwCreateWindow(screenWidth, screenHeight, "Frame Buffer", monitor, NULL);

    glfwMakeContextCurrent(_this.window);

    glCreateShader(GL_VERTEX_SHADER);

    /* The next line, when uncommented, removes the farmerrate cap that matchs the screen regresh rate. */
    /* glfwSwapInterval(0); */
    glfwSetFramebufferSizeCallback(_this.window, framebuffer_size_callback);

    double ratioX = ((float)_this.imageData.size.x / (float)_this.imageData.size.y) / ((float)screenWidth / (float)screenHeight);
    double ratioY = 1.0;
    /* set up vertex data (and buffer(s)) and configure vertex attributes
     ------------------------------------------------------------------*/
    float vertices[] = {
        /* positions       /*texture coords */
        ratioX * 1.0f, ratioY * 1.0f, 0.0f, 1.0f, 0.0f,   /* top right */
        ratioX * 1.0f, ratioY * -1.0f, 0.0f, 1.0f, 1.0f,  /* bottom right */
        ratioX * -1.0f, ratioY * -1.0f, 0.0f, 0.0f, 1.0f, /* bottom left */
        ratioX * -1.0f, ratioY * 1.0f, 0.0f, 0.0f, 0.0f   /* top left */
    };
    unsigned int indices[] = {
        0, 1, 3, /* first triangle */
        1, 2, 3  /* second triangle */
    };
    textureCreate(&_this);

	_this.shaderProgram = shaderProgramCreateFromFiles("assets/shaders/default.vs", "assets/shaders/default.fs");

	glUseProgram(_this.shaderProgram);
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &_this.VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(_this.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    /*  position attribute */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    /* texture coord attribute */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glfwSetInputMode(_this.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPos(_this.window, 0, 0);
    return _this;
}

void graphicsSwapBuffers(Graphics _this)
{
    static Color *lastPointer = NULL;

    if (!lastPointer)
        lastPointer = _this.imageData.data;

    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    /* Update texture */
    glBindTexture(GL_TEXTURE_2D, _this.textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _this.imageData.size.x, _this.imageData.size.y, GL_RGB, GL_UNSIGNED_BYTE, _this.imageData.data);
    /* render container */
    glBindVertexArray(_this.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void graphicsDestroy(Graphics _this)
{
    glfwSetWindowShouldClose(_this.window, true);
    FREE(_this.imageData.data);
    glfwDestroyWindow(_this.window);
    glfwTerminate();
}

Color graphicsGetPixel(ImageData _this, PointI point)
{
    int position = (point.x + point.y * _this.size.x) % _this.bufferSize;
    return _this.data[position];
}

void graphicsClear(ImageData _this)
{
    memset(_this.data, 0, _this.bufferSize);
}

void graphicsClearColor(ImageData _this, Color color)
{
	int bufferSize = _this.bufferSize / sizeof(Color);
	size_t i = 0;
	for(i = 0; i < bufferSize; i++)
	{
		_this.data[i] = color;
	}
}

void graphicsUpdateMouseCoordinates(Graphics *_this)
{
    int w, h;
    double mouseX, mouseY;
    glfwGetWindowSize(_this->window, &w, &h);
    glfwGetCursorPos(_this->window, &mouseX, &mouseY);
    _this->mousePosition.x = mouseX + _this->imageData.size.x / (float)w;
    _this->mousePosition.y = mouseY + _this->imageData.size.y / (float)h;
    _this->mouseRightDown = glfwGetMouseButton(_this->window, GLFW_MOUSE_BUTTON_1);
}
