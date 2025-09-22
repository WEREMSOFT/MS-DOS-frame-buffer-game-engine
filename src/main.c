#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <soloud_c.h>
#define INITIAL_LEVEL GAME_STATE_CLICK_TO_START
#define UR_SCREEN_WIDTH 320
#define UR_SCREEN_HEIGHT 240

#define UR_MALLOC malloc
#define UR_REALLOC realloc
#define UR_FREE free

void urPutPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

#define UR_PUT_PIXEL urPutPixel
#include "universal_renderer.h"

#define Color URColor
#define PointI URPointI
#define PointF URPointF

#include "core/utils/utils.h"
#include "core/input/keyboard.h"

ImageData globalImgData;

void urPutPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	int position = (x + y * UR_SCREEN_WIDTH) % globalImgData.bufferSize;
	globalImgData.data[position] = (URColor){r, g, b};
}

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#define ARRAY_MALLOC allocStatic
#define ARRAY_REALLOC reallocStatic
#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "core/array_t/array_t.h"

#define STBI_MALLOC(sz) malloc(sz)
#define STBI_REALLOC(p, newsz) realloc(p, newsz)
#define STBI_FREE(p) freefree(p)

typedef enum
{
	ASSET_NONE,
	ASSET_COUNT
} Assets;

struct GameState;

typedef enum
{
	GAME_STATE_CLICK_TO_START,
	GAME_STATE_LEVEL1_INIT,
	GAME_STATE_LEVEL1_TUTORIAL,
	GAME_STATE_LEVEL1_PLAY,
	GAME_STATE_LEVEL1_EXIT,
	GAME_STATE_COUNT
} GameStateEnum;

typedef struct GameState
{
	struct GameState *_self;
	Graphics graphics;
	URSprite sprites[ASSET_COUNT];
	float deltaTime;
	bool shouldQuit;
	bool shouldStop;
	bool showDemoWindow;
	GameStateEnum gameStateEnum;
} GameState;

GameState gameStateCheckExitConditions(GameState _this)
{
	if (glfwWindowShouldClose(_this.graphics.window))
	{
		_this.shouldQuit = true;
		return _this;
	}

	_this.shouldQuit = isKeyJustPressed(_this.graphics.window, GLFW_KEY_ESCAPE);
	if (isKeyJustPressed(_this.graphics.window, GLFW_KEY_ENTER))
		_this.shouldStop = true;

	return _this;
}

void swapBuffersPrintFPSPollEvents(Graphics graphics, float deltaTime)
{
	urPrintFPS(deltaTime);
	graphicsSwapBuffers(graphics);
	glfwPollEvents();
}

void loadAssets(URSprite *_this)
{
}

GameState gameStateClickToStart(GameState _this)
{
	graphicsClear(_this.graphics.imageData);
	URColor static textColor = {255, 255, 255};
	char *clickToStart = "click to start";

	static int gameColor = 0;
	static float elapsedTime = 0;

	elapsedTime += getDeltaTime();

	if (elapsedTime > 0.0001)
	{
		elapsedTime = 0;
		gameColor = (gameColor + 1) % 3;
		switch (gameColor)
		{
		case 0:
			textColor.r = 255;
			textColor.g = 255;
			textColor.b = 255;
			break;
		case 1:
			textColor.r = 0;
			textColor.g = 0;
			textColor.b = 255;
			break;
		case 2:
			textColor.r = 255;
			textColor.g = 0;
			textColor.b = 0;
			break;
		}
	}

	urPrintString((URPointI){120, 120}, clickToStart, textColor);
	if (glfwGetMouseButton(_this.graphics.window, GLFW_MOUSE_BUTTON_1))
		_this.shouldStop = true;
	return _this;
}

GameState gameMainLoop(GameState gameState)
{
	globalImgData = gameState.graphics.imageData;
	gameState.deltaTime = getDeltaTime();
	gameState = gameStateCheckExitConditions(gameState);

	switch (gameState.gameStateEnum)
	{
	case GAME_STATE_CLICK_TO_START:
		gameState = gameStateClickToStart(gameState);
		break;
	default:
		gameState.shouldQuit = true;
	}
	if (gameState.shouldStop)
	{
		gameState.shouldStop = false;
		gameState.gameStateEnum++;
	}
	swapBuffersPrintFPSPollEvents(gameState.graphics, gameState.deltaTime);
	glfwSwapBuffers(gameState.graphics.window);

	return gameState;
}

#ifdef __EMSCRIPTEN__
GameState gameState = {0};

void emscriptenLoopHandler()
{
	gameState = gameMainLoop(gameState);
}
#endif

int main(void)
{
	initDeltaTime();

#ifndef __EMSCRIPTEN__
	GameState gameState = {0};
#endif

	gameState._self = &gameState;
	gameState.graphics = graphicsCreate(320, 240, false);

	loadAssets(gameState.sprites);

#ifdef INITIAL_LEVEL
	gameState.gameStateEnum = INITIAL_LEVEL;
#endif

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(emscriptenLoopHandler, 0, false);
#else

	while (!gameState.shouldStop && !gameState.shouldQuit)
		gameState = gameMainLoop(gameState);

Cleanup:
	graphicsDestroy(gameState.graphics);
	return 0;
#endif
}
