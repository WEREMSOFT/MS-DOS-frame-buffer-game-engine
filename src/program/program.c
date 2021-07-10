#include "program.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "core/input/input.h"
#include "utils/utils.h"

#define __STATIC_ALLOC_IMPLEMENTATION__
#include "core/stackAllocator/staticAlloc.h"

#ifdef ARRAY_MALLOC
#undef ARRAY_MALLOC
#endif

#ifdef ARRAY_REALLOC
#undef ARRAY_REALLOC
#endif

#define ARRAY_MALLOC allocStatic

#define ARRAY_REALLOC reallocStatic

#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "core/array/array.h"

#define STBI_MALLOC(sz) allocStatic(sz)
#define STBI_REALLOC(p, newsz) reallocStatic(p, newsz)
#define STBI_FREE(p) freeStatic(p)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Program
programCreate()
{
    staticAllocatorInit(100092024);
    Program this = {0};
    this.graphics = graphicsCreate();
    this.sound = soundCreate();
    Soloud_setGlobalVolume(this.sound.soloud, 0);
    this.textEditor = textEditorCreate(this.graphics, this.sound);

    return this;
}

void programMainLoop(Program this)
{
    textEditorUpdate(this.textEditor);
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
    staticAllocatorDestroy();
}
