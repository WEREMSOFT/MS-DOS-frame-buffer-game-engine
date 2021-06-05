#include "program.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "shader/shader.h"
#include <math.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void textureCreateFromImage(Program *this)
{
    int width, height, nrChannels;

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    // Set texture wrapping filtering options.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(false);
    this->textureData = stbi_load("assets/320x200.png", &width, &height, &nrChannels, 0);

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
        printf("%s::%s - Error loading texture \n");
        printf("Error: %s\n", stbi_failure_reason());

        exit(1);
    }
}

static void textureCreate(Program *this)
{

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    this->textureData = malloc(TEXTURE_WIDTH * TEXTURE_HEIGHT * sizeof(Color));
    printf("%p\n", this->textureData);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, this->textureData);
}

Program programCreate()
{
    Program this = {0};

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

    if (err != GLEW_OK)
    {
        fprintf(stderr, "error initializing glew\n");
        exit(-1);
    }

    double ratioX = (TEXTURE_WIDTH / TEXTURE_HEIGHT) / ((float)mode->width / (float)mode->height);
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

    // textureCreate(&this);
    textureCreateFromImage(&this);

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

    return this;
}

int x = 100;
int y = 100;

enum ShapeType
{
    CIRCLE,
    SQUARE,
    POINT,
    SHAPE_TYPE_COUNT
};

int shapeType = CIRCLE;
bool shouldClear = false;
void programMainLoop(Program this)
{
    double refreshRate = 0;
    double lastUpdate = glfwGetTime();
    double currentFPS = 0;
    while (!glfwWindowShouldClose(this.window))
    {

        if (glfwGetKey(this.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(this.window, true);
        }

        if (glfwGetKey(this.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            y--;
        }

        if (glfwGetKey(this.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            y++;
        }

        if (glfwGetKey(this.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            x--;
        }

        if (glfwGetKey(this.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            x++;
        }

        if (glfwGetKey(this.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            shapeType = (shapeType + 1) % (SHAPE_TYPE_COUNT);
        }

        if (glfwGetKey(this.window, GLFW_KEY_C) == GLFW_PRESS)
        {
            shouldClear = !shouldClear;
        }

        y = fmax(0, fmin(y, TEXTURE_HEIGHT - 1));
        x = fmax(0, fmin(x, TEXTURE_WIDTH - 1));
        if (shouldClear)
            clear(this.textureData);
        switch (shapeType)
        {
        case POINT:
            putPixel(this.textureData, x, y, (Color){255, 0, 0});
            break;
        case SQUARE:
            drawSquare(this.textureData, x, y, 50, 50, (Color){random() % 255, random() % 255, 0});
            break;
        case CIRCLE:
            drawCircle(this.textureData, x, y, 50, (Color){random() % 255, random() % 255, 0});
        }
        // glClearColor(0, 0, 0, 1.0);
        // glClear(GL_COLOR_BUFFER_BIT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, this.textureData);

        // bind textures on corresponding texture units
        glBindTexture(GL_TEXTURE_2D, this.textureId);

        // render container
        glBindVertexArray(this.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(this.window);
        glfwPollEvents();
        currentFPS = 1 / (glfwGetTime() - lastUpdate);
        char windowTitle[1000] = {0};
        snprintf(windowTitle, 1000, "\r fps: %f", floor(currentFPS));
        glfwSetWindowTitle(this.window, windowTitle);
        lastUpdate = glfwGetTime();
    }
}

void programDestroy(Program this)
{
    free(this.textureData);
    glfwDestroyWindow(this.window);
    glfwTerminate();
}