#include "graphics.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "../shader/shader.h"
#include "../stackAllocator/staticAlloc.h"
#include <stb_image.h>

extern char fonts[][5];

static void textureCreate(Graphics *this)
{

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    this->imageData.bufferSize = this->imageData.size.x * this->imageData.size.y * sizeof(Color);
    this->imageData.data = allocStatic(this->imageData.bufferSize);
    printf("%p\n", this->imageData.data);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imageData.size.x, this->imageData.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, this->imageData.data);
}

void textureCreateFromImage(Graphics *this, char *fileName)
{
    int width, height, nrChannels;

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    // Set texture wrapping filtering options.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(false);
    this->imageData.data = (Color *)stbi_load(fileName, &width, &height, &nrChannels, 0);

    if (this->imageData.data)
    {

        GLenum format = 0;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, this->imageData.data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("%s::%s - Error loading texture \n", __FILE__, __FUNCTION__);
        printf("Error: %s\n", stbi_failure_reason());

        exit(1);
    }
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

Graphics graphicsCreate()
{
    Graphics this = {0};
    this.imageData.size.x = 640;
    this.imageData.size.y = 480;

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

    this.window = glfwCreateWindow(mode->width, mode->height, "Frame Buffer", monitor, NULL);
    glfwMakeContextCurrent(this.window);
    // The next line, when uncommented, removes the farmerrate cap that matchs the screen regresh rate.
    // glfwSwapInterval(2);
    glfwSetFramebufferSizeCallback(this.window, framebuffer_size_callback);

    GLenum err = glewInit();

    if (err != GLEW_OK || err == 255) // the 255 part is because Wayland fails even if the initialization went right
    {
        fprintf(stderr, "error initializing glew: %s\n", glewGetErrorString(err));
        exit(-1);
    }

    double ratioX = ((float)this.imageData.size.x / (float)this.imageData.size.y) / ((float)mode->width / (float)mode->height);
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

    textureCreate(&this);

    this.shaderProgram = shaderProgramCreateFromFiles("default.vs", "default.fs");
    glUseProgram(this.shaderProgram);

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &this.VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(this.VAO);

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

    glfwSetInputMode(this.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetCursorPos(this.window, 0, 0);

    return this;
}

void graphicsSwapBuffers(Graphics this)
{
    static Color *lastPointer = NULL;

    if (!lastPointer)
        lastPointer = this.imageData.data;

    if (lastPointer != this.imageData.data)
    {
        fprintf(stderr, "pointers are different!!\n");
        exit(-1);
    }
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update texture
    glBindTexture(GL_TEXTURE_2D, this.textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this.imageData.size.x, this.imageData.size.y, GL_RGB, GL_UNSIGNED_BYTE, this.imageData.data);

    // render container
    glBindVertexArray(this.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(this.window);
}

void graphicsDestroy(Graphics this)
{
    glfwSetWindowShouldClose(this.window, true);
    freeStatic(this.imageData.data);
    glfwDestroyWindow(this.window);
    glfwTerminate();
}

void graphicsPutPixel(ImageData this, PointI point, Color color)
{
    int position = (point.x + point.y * this.size.x) % this.bufferSize;
    this.data[position] = color;
}

inline Color graphicsGetPixel(ImageData this, PointI point)
{
    int position = (point.x + point.y * this.size.x) % this.bufferSize;
    return this.data[position];
}

void graphicsDrawCircle(ImageData this, PointI center, double radious, Color color)
{
    for (int i = center.x - radious; i <= center.x + radious; i++)
    {
        for (int j = center.y - radious; j <= center.y + radious; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                graphicsPutPixel(this, (PointI){i, j}, color);
        }
    }
}

void graphicsDrawSquare(ImageData this, PointI topLeftCorner, PointI size, Color color)
{
    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {
        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {
            if (j == topLeftCorner.y || j == topLeftCorner.y + size.y || i == topLeftCorner.x || i == topLeftCorner.x + size.x)
                graphicsPutPixel(this, (PointI){i, j}, color);
        }
    }
}

void graphicsDrawCircleFill(ImageData this, PointI center, double radious, Color color)
{
    for (int i = center.x - radious; i <= center.x + radious; i++)
    {
        for (int j = center.y - radious; j <= center.y + radious; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                graphicsPutPixel(this, (PointI){i, j}, color);
        }
    }
}

void graphicsDrawSquareFill(ImageData this, PointI topLeftCorner, PointI size, Color color)
{
    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {
        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {
            graphicsPutPixel(this, (PointI){i, j}, color);
        }
    }
}

void graphicsClear(ImageData this)
{
    memset(this.data, 0, this.bufferSize);
}

void graphicsDrawCharacter(ImageData this, PointI topLeftCorner, unsigned int letter, Color color)
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j <= 8; j++)
        {
            if (fonts[letter][i] & (0b1000000 >> j))
                graphicsPutPixel(this, (PointI){topLeftCorner.x + j, topLeftCorner.y + i}, color);
        }
    }
}

void graphicsPrintFontTest(ImageData this)
{
    for (int i = 0; i < 38; i++)
    {
        graphicsDrawCharacter(this, (PointI){i * 6, 100}, i, (Color){0xff, 0xff, 0xff});
    }
}

void graphicsPrintString(ImageData this, PointI topLeftCorner, char *string, Color color)
{
    size_t stringLen = strlen(string);
    for (size_t i = 0; i < stringLen; i++)
    {
        if (string[i] >= '0' && string[i] <= '9')
        {
            graphicsDrawCharacter(this, (PointI){topLeftCorner.x + i * 6, topLeftCorner.y}, string[i] - '0', color);
        }
        else if (string[i] >= 'a' && string[i] <= 'z')
        {
            int charOffset = string[i] - 'a' + 10;
            graphicsDrawCharacter(this, (PointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
        }
    }
}

void graphicsUpdateMouseCoordinates(Graphics *this)
{
    int w, h;
    double mouseX, mouseY;
    glfwGetWindowSize(this->window, &w, &h);
    glfwGetCursorPos(this->window, &mouseX, &mouseY);
    this->mousePosition.x = mouseX + this->imageData.size.x / (float)w;
    this->mousePosition.y = mouseY + this->imageData.size.y / (float)h;
    this->mouseRightDown = glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_1);
}

void graphicsDrawLine(ImageData this, PointI pointA, PointI pointB, Color color)
{

    int dx = abs(pointB.x - pointA.x), sx = pointA.x < pointB.x ? 1 : -1;
    int dy = abs(pointB.y - pointA.y), sy = pointA.y < pointB.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        graphicsPutPixel(this, pointA, color);
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