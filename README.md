# Old School Frame Buffer game engine
Old school, MS-DOS golden era game engine where you create images by writing into a memory region, that is pushed to the video card.

# Dependencies
GLFW

# Usage
Read the main.c, program.c (function mainLoop) and the graphics.h to get at basic understanding of how to use it(should be enough).

# ONLINE DEMO

https://weremsoft.github.io/MS-DOS-frame-buffer-game-engine/

# IMPORTANT NOTES ON DEARIMGUI FOR WEB
I'm using cimgui, whici is a C wrapper for dearimgui (which is written in C++). For this project, I use static libraries both, for the desktop version, and for the web version. The desktop version static library(included as a libcimgui.a file) is intended to be used on linux (or WSSL) and the web static library compiled with emscripten (inclided as cimgui.a file) is intended to be used in the browser (firefox or chrome).

IF YOU NEED TO COMPILE THE CIMGUI PROJECT TO BE USED WITH WEBASSEMBLY THIS IS HOW:

```
mkdir build
cd build
emcmake cmake .. -DIMGUI_STATIC=on -DCMAKE_BUILD_TYPE=Release
make -j 4
```