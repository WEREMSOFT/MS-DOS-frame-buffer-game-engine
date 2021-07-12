#include "textEditor.h"
#include <stdlib.h>
#include "../core/graphics/graphics.h"
#include "../utils/utils.h"
#include "../core/input/keyboard.h"
#include "../core/sprite/sprite.h"
#include "../core/stackAllocator/staticAlloc.h"
#include <math.h>
char keyStatusText[100] = {0};
char *bufferText;
int bufferTextIndex = 0;
void keyboardCallback(GLFWwindow *window, unsigned int key)
{
    snprintf(keyStatusText, 100, "key: %d", key);

    if (key >= 'A' && key <= 'Z')
        bufferText[bufferTextIndex++] = (char)key + 32;
    else
        bufferText[bufferTextIndex++] = (char)key;
}

void keyCallback(GLFWwindow *window, int key, int code, int action, int mod)
{
    if (action != GLFW_PRESS)
        return;
    if (key == GLFW_KEY_TAB)
    {
        bufferText[bufferTextIndex++] = '\t';
    }
    if (key == GLFW_KEY_BACKSPACE)
    {
        bufferTextIndex = fmax(0, --bufferTextIndex);
        bufferText[bufferTextIndex] = 0;
    }
    else if (key == GLFW_KEY_ENTER)
    {
        bufferText[bufferTextIndex++] = '\n';
    }
}

TextEditor textEditorCreate(Graphics graphics, Sound sound)
{
    TextEditor this = {0};
    this.sound = sound;
    this.graphics = graphics;
    bufferText = allocStatic(100000);
    glfwSetCharCallback(graphics.window, keyboardCallback);
    glfwSetKeyCallback(graphics.window, keyCallback);

    this.shouldQuit = false;
    return this;
}

TextEditor textEditorUpdate(TextEditor this)
{
    const Color color = (Color){0, 0, 0xFF};
    bool shouldContinue = true;
    while (shouldContinue && !this.shouldQuit)
    {
        imClearColor(this.graphics.imageData, color);
        this.shouldQuit = isKeyJustPressed(this.graphics.window, GLFW_KEY_ESCAPE);
        imPrintString(this.graphics.imageData, (PointI){0, 10}, keyStatusText, (Color){0xAA, 0xFF, 0xFF});
        graphicsPrintString(this.graphics, (PointI){0, 20}, bufferText, (Color){0xFF, 0xFF, 0xFF});
        printFPS(this.graphics, getDeltaTime());
        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }

    return this;
}

void textEditorDestroy()
{
    freeStatic(bufferText);
}