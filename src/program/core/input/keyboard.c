#include "keyboard.h"
#include <SDL.h>
static int keys[255] = {0};

bool isKeyJustPressed(SDL_Window *window, int key)
{
    int actualKeyState = glfwGetKey(window, key);

    if (keys[key] == GLFW_PRESS && actualKeyState == GLFW_PRESS)
        return false;

    keys[key] = actualKeyState;

    return actualKeyState == GLFW_PRESS;
}