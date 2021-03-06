#include "graphics.h"
#include <stb_image.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "../shader/shader.h"

extern char fonts[][5];

static void textureCreate(Graphics *this)
{

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    this->textureData = malloc(this->textureWidth * this->textureHeight * sizeof(Color));
    printf("%p\n", this->textureData);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->textureWidth, this->textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, this->textureData);
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
    this->textureData = (Color *)stbi_load(fileName, &width, &height, &nrChannels, 0);

    if (this->textureData)
    {

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, this->textureData);
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
    this.textureWidth = 320;
    this.textureHeight = 200;

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
    glfwSetFramebufferSizeCallback(this.window, framebuffer_size_callback);

    GLenum err = glewInit();

    if (err != GLEW_OK || err == 255) // the 255 part is because Wayland fails even if the initialization went right
    {
        fprintf(stderr, "error initializing glew: %s\n", glewGetErrorString(err));
        exit(-1);
    }

    double ratioX = ((float)this.textureWidth / (float)this.textureHeight) / ((float)mode->width / (float)mode->height);
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

    this.shaderProgram = shaderProgramCreateFromFiles("assets/shaders/default.vs", "assets/shaders/default.fs");
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
        lastPointer = this.textureData;

    if (lastPointer != this.textureData)
    {
        fprintf(stderr, "pointers are different!!\n");
        exit(-1);
    }
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update texture
    glBindTexture(GL_TEXTURE_2D, this.textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this.textureWidth, this.textureHeight, GL_RGB, GL_UNSIGNED_BYTE, this.textureData);

    // render container
    glBindVertexArray(this.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(this.window);
}

void graphicsDestroy(Graphics this)
{
    free(this.textureData);
    glfwDestroyWindow(this.window);
    glfwTerminate();
}

void graphicsPutPixel(Graphics this, PointI point, Color color)
{
    int position = (point.x + point.y * this.textureWidth) % 64000;
    this.textureData[position] = color;
}

void graphicsDrawCircle(Graphics this, PointI center, double radious, Color color)
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

void graphicsDrawSquare(Graphics this, PointI topLeftCorner, PointI size, Color color)
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

void graphicsDrawCircleFill(Graphics this, PointI center, double radious, Color color)
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

void graphicsDrawSquareFill(Graphics this, PointI topLeftCorner, PointI size, Color color)
{
    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {
        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {
            graphicsPutPixel(this, (PointI){i, j}, color);
        }
    }
}

void graphicsClear(Graphics this)
{
    memset(this.textureData, 0, this.textureWidth * this.textureHeight * sizeof(Color));
}

void graphicsDrawCharacter(Graphics this, PointI topLeftCorner, unsigned int letter, Color color)
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

void graphicsPrintFontTest(Graphics this)
{
    for (int i = 0; i < 38; i++)
    {
        graphicsDrawCharacter(this, (PointI){i * 6, 100}, i, (Color){0xff, 0xff, 0xff});
    }
}

void graphicsPrintString(Graphics this, PointI topLeftCorner, char *string, Color color)
{
    unsigned int stringLen = strlen(string);
    for (int i = 0; i < stringLen; i++)
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
    this->mousePosition.x = mouseX + this->textureWidth / (float)w;
    this->mousePosition.y = mouseY + this->textureHeight / (float)h;
    this->mouseRightDown = glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_1);
}

void graphicsDrawLine(Graphics this, PointI pointA, PointI pointB, Color color)
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
