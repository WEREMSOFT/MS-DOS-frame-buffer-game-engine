#include <glad/glad.h>
#include "graphics.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "../shader/shader.h"
#include "../stackAllocator/staticAlloc.h"

extern char fonts[][5];
static void textureCreate(Graphics *_this)
{
    glGenTextures(1, &_this->textureId);
    glBindTexture(GL_TEXTURE_2D, _this->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    _this->imageData.bufferSize = _this->imageData.size.x * _this->imageData.size.y * sizeof(Color);
    _this->imageData.data = allocStatic(_this->imageData.bufferSize);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _this->imageData.size.x, _this->imageData.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, _this->imageData.data);
}

// void textureCreateFromImage(Graphics *_this, char *fileName)
// {
//     int width, height, nrChannels;
//     glGenTextures(1, &_this->textureId);
//     glBindTexture(GL_TEXTURE_2D, _this->textureId);
//     // Set texture wrapping filtering options.
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     stbi_set_flip_vertically_on_load(false);
//     _this->imageData.data = (Color *)stbi_load(fileName, &width, &height, &nrChannels, 0);

//     if (_this->imageData.data)
//     {
//         GLenum format = 0;

//         if (nrChannels == 1)
//             format = GL_RED;
//         else if (nrChannels == 3)
//             format = GL_RGB;
//         else if (nrChannels == 4)
//             format = GL_RGBA;
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, _this->imageData.data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
// }

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
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

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    // loadOpenGLFunctions();
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glCreateShader(GL_VERTEX_SHADER);

    // The next line, when uncommented, removes the farmerrate cap that matchs the screen regresh rate.
    // glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(_this.window, framebuffer_size_callback);

    double ratioX = ((float)_this.imageData.size.x / (float)_this.imageData.size.y) / ((float)screenWidth / (float)screenHeight);
    double ratioY = 1.0;
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions       // texture coords
        ratioX * 1.0f, ratioY * 1.0f, 0.0f, 1.0f, 0.0f,   // top right
        ratioX * 1.0f, ratioY * -1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
        ratioX * -1.0f, ratioY * -1.0f, 0.0f, 0.0f, 1.0f, // bottom left
        ratioX * -1.0f, ratioY * 1.0f, 0.0f, 0.0f, 0.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    textureCreate(&_this);
#ifdef __EMSCRIPTEN__
    _this.shaderProgram = shaderProgramCreateFromFiles("assets/shaders/defaultWeb.vs", "assets/shaders/defaultWeb.fs");
#else
    _this.shaderProgram = shaderProgramCreateFromFiles("assets/shaders/default.vs", "assets/shaders/default.fs");
#endif
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
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
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
    // Update texture
    glBindTexture(GL_TEXTURE_2D, _this.textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _this.imageData.size.x, _this.imageData.size.y, GL_RGB, GL_UNSIGNED_BYTE, _this.imageData.data);
    // render container
    glBindVertexArray(_this.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(_this.window);
}

void graphicsDestroy(Graphics _this)
{
    glfwSetWindowShouldClose(_this.window, true);
    freeStatic(_this.imageData.data);
    glfwDestroyWindow(_this.window);
    glfwTerminate();
}

void graphicsPutPixel(ImageData _this, PointI point, Color color)
{
    int position = (point.x + point.y * _this.size.x) % _this.bufferSize;
    _this.data[position] = color;
}

inline Color graphicsGetPixel(ImageData _this, PointI point)
{
    int position = (point.x + point.y * _this.size.x) % _this.bufferSize;
    return _this.data[position];
}

void graphicsDrawCircle(ImageData _this, PointI center, double radious, Color color)
{

    for (int i = center.x - radious; i <= center.x + radious; i++)
    {

        for (int j = center.y - radious; j <= center.y + radious; j++)
        {

            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                graphicsPutPixel(_this, (PointI){i, j}, color);
        }
    }
}

void graphicsDrawSquare(ImageData _this, PointI topLeftCorner, PointI size, Color color)
{

    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {

        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {

            if (j == topLeftCorner.y || j == topLeftCorner.y + size.y || i == topLeftCorner.x || i == topLeftCorner.x + size.x)
                graphicsPutPixel(_this, (PointI){i, j}, color);
        }
    }
}

void graphicsDrawCircleFill(ImageData _this, PointI center, double radious, Color color)
{

    for (int i = center.x - radious; i <= center.x + radious; i++)
    {

        for (int j = center.y - radious; j <= center.y + radious; j++)
        {

            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                graphicsPutPixel(_this, (PointI){i, j}, color);
        }
    }
}

void graphicsDrawSquareFill(ImageData _this, PointI topLeftCorner, PointI size, Color color)
{

    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {

        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {
            graphicsPutPixel(_this, (PointI){i, j}, color);
        }
    }
}

void graphicsClear(ImageData _this)
{
    memset(_this.data, 0, _this.bufferSize);
}

void graphicsDrawCharacter(ImageData _this, PointI topLeftCorner, unsigned int letter, Color color)
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j <= 8; j++)
        {
            if (fonts[letter][i] & (0b1000000 >> j))
                graphicsPutPixel(_this, (PointI){topLeftCorner.x + j, topLeftCorner.y + i}, color);
        }
    }
}

void graphicsPrintFontTest(ImageData _this)
{

    for (int i = 0; i < 38; i++)
    {
        graphicsDrawCharacter(_this, (PointI){i * 6, 100}, i, (Color){0xff, 0xff, 0xff});
    }
}

// void graphicsPrintString(ImageData _this, PointI topLeftCorner, char *string, Color color)
// {
//     size_t stringLen = strlen(string);

//     for (size_t i = 0; i < stringLen; i++)
//     {
//         if (string[i] == '.')
//         {
//             int charOffset = 'z' - 'a' + 12;
//             graphicsDrawCharacter(_this, (PointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
//             continue;
//         }

//         if (string[i] == '%')
//         {
//             int charOffset = 'z' - 'a' + 11;
//             graphicsDrawCharacter(_this, (PointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
//             continue;
//         }

//         if (string[i] >= '0' && string[i] <= '9')
//         {
//             graphicsDrawCharacter(_this, (PointI){topLeftCorner.x + i * 6, topLeftCorner.y}, string[i] - '0', color);
//             continue;
//         }

//         if (string[i] >= 'a' && string[i] <= 'z')
//         {
//             int charOffset = string[i] - 'a' + 10;
//             graphicsDrawCharacter(_this, (PointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
//             continue;
//         }
//     }
// }

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

void graphicsDrawLine(ImageData _this, PointI pointA, PointI pointB, Color color)
{
    int dx = abs(pointB.x - pointA.x), sx = pointA.x < pointB.x ? 1 : -1;
    int dy = abs(pointB.y - pointA.y), sy = pointA.y < pointB.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        graphicsPutPixel(_this, pointA, color);

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

inline PointI pointFToPointI(PointF source)
{
    return (PointI){(int)source.x, (int)source.y};
}
