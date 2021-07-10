#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/graphics/graphics.h"
#include "core/sprite/sprite.h"
#include "textEditor/textEditor.h"
#include "sound/sound.h"

typedef struct
{
    Graphics graphics;
    TextEditor textEditor;
    Sound sound;
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif