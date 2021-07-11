#ifndef __TEXT_EDITOR_H__
#define __TEXT_EDITOR_H__

#include "../core/graphics/graphics.h"
#include "../core/sprite/sprite.h"
#include "../sound/sound.h"

typedef struct
{
    Graphics graphics;
    Sound sound;
    bool shouldQuit;
} TextEditor;

TextEditor textEditorCreate(Graphics graphics, Sound sound);
TextEditor textEditorUpdate();

#endif