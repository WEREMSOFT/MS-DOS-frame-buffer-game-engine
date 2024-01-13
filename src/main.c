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
#define UR_SCREEN_HEIGHT 320

#define __STATIC_ALLOC_IMPLEMENTATION__
#include "program/core/stackAllocator/staticAlloc.h"

#define UR_MALLOC allocStatic
#define UR_REALLOC reallocStatic
#define UR_FREE freeStatic
unsigned char *imageData;
#define urPutPixelM(x, y, r, g, b)                   \
	{                                                \
		int index = ((x) + (y)*UR_SCREEN_WIDTH) * 3; \
		if (index > -1)                              \
		{                                            \
			imageData[index] = (r);                  \
			imageData[index + 1] = (g);              \
			imageData[index + 2] = (b);              \
		}                                            \
	}

#define UR_PUT_PIXEL urPutPixelM
#include "universal_renderer.h"
#define Color URColor
#define PointI URPointI
#define PointF URPointF

#include "program/core/utils/utils.h"
#include "program/core/input/keyboard.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#define ARRAY_MALLOC allocStatic
#define ARRAY_REALLOC reallocStatic
#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "program/core/array/array.h"

#define STBI_MALLOC(sz) allocStatic(sz)
#define STBI_REALLOC(p, newsz) reallocStatic(p, newsz)
#define STBI_FREE(p) freeStatic(p)

#define SAMPLE_IMAGE_COUNT 3
#define SAMPLE_SIZE (5*5*SAMPLE_IMAGE_COUNT)

#define SAMPLE_WIDTH 320
#define SAMPLE_HEIGHT 320

typedef enum
{
	ASSET_NONE,
	ASSET_NOISE_A,
	ASSET_NOISE_B,
	ASSET_MODEL_FACE01,
	ASSET_MODEL_FACE02,
	ASSET_MODEL_FACE03,
	ASSET_MODEL_CHECKER,
	ASSET_COUNT
} Assets;

struct GameState;

typedef enum
{
	GAME_STATE_CLICK_TO_START,
	GAME_STATE_BUILD_DATA_MODEL,
	GAME_STATE_GENERATE_IMAGE,
	GAME_STATE_COUNT
} GameStateEnum;

typedef struct
{
	URColor color;
	double hash;
} DataSet;

typedef struct GameState
{
	struct GameState *_self;
	Graphics graphics;
	URSprite sprites[ASSET_COUNT];
	DataSet dataSets[SAMPLE_SIZE];
	float deltaTime;
	bool shouldQuit;
	bool shouldStop;
	URSprite currentFrame;
	URSprite nextFrame;
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
	_this[ASSET_NOISE_A] = urSpriteCreateEmpty((URPointI){UR_SCREEN_WIDTH, UR_SCREEN_HEIGHT}, (URColor){255, 0, 0});
	_this[ASSET_NOISE_B] = urSpriteCreateEmpty((URPointI){UR_SCREEN_WIDTH, UR_SCREEN_HEIGHT}, (URColor){0, 0, 255});

	static uint32_t seed = 23423423423;

	for(int x = 0; x < UR_SCREEN_WIDTH; x++)
		for(int y = 0; y < UR_SCREEN_HEIGHT; y++)
			urSpritePutPixel(_this[ASSET_NOISE_A], (URPointI){x, y}, (URColor){pcgRandom(++seed) * 255, pcgRandom(++seed) * 255, pcgRandom(++seed) * 255});

	_this[ASSET_MODEL_FACE01] = urSpriteCreate("assets/AI/face01.bmp");
	_this[ASSET_MODEL_FACE02] = urSpriteCreate("assets/AI/face02.bmp");
	_this[ASSET_MODEL_FACE03] = urSpriteCreate("assets/AI/face03.bmp");
	_this[ASSET_MODEL_CHECKER] = urSpriteCreate("assets/AI/checkers.bmp");

}

float ratio = (float)(SAMPLE_WIDTH*SAMPLE_HEIGHT)/(float)(UR_SCREEN_WIDTH * UR_SCREEN_HEIGHT);

double calculatePixelHash(URSprite _this, URPointI position, bool resetCounter)
{

	static int counter = 0;

	if(resetCounter) 
		counter = 0;

	counter++;

	uint32_t avgPixel[8] = {0};
	// TOP
	{
		URColor pixel = urSpriteGetPixel(_this, (URPointI){position.x-1, position.y-1});
		uint32_t component = (pixel.r);
		// uint32_t component = (pixel.r + pixel.g + pixel.b);
		avgPixel[0] = component;
	}

	{
		URColor pixel = urSpriteGetPixel(_this, (URPointI){position.x, position.y-1});
		uint32_t component = (pixel.r);
		avgPixel[1] = component;
	}

	{
		URColor pixel = urSpriteGetPixel(_this, (URPointI){position.x+1, position.y-1});
		uint32_t component = (pixel.r);
		avgPixel[2] = component;
	}

	// MIDDLE
	{
		URColor pixel = urSpriteGetPixel(_this, (URPointI){position.x-1, position.y});
		uint32_t component = (pixel.r);
		avgPixel[3] = component;
	}

	{
		URColor pixel = urSpriteGetPixel(_this, (URPointI){position.x+1, position.y});
		uint32_t component = (pixel.r);
		avgPixel[4] = component;
	}

	// BOTTOM
	{
		URColor pixel = urSpriteGetPixel(_this, (URPointI){position.x-1, position.y+1});
		uint32_t component = (pixel.r);
		avgPixel[5] = component;
	}
	{
		URColor pixel = urSpriteGetPixel(_this, (URPointI){position.x, position.y+1});
		uint32_t component = (pixel.r);
		avgPixel[6] = component;
	}

	{
		URColor pixel = urSpriteGetPixel(_this, (URPointI){position.x+1, position.y+1});
		uint32_t component = (pixel.r);
		avgPixel[7] = component;
	}

	double result = 0;

	// URPointF normalizedPosition = {0};

	// normalizedPosition.x = position.x / (float) _this.size.x;
	// normalizedPosition.y = position.y / (float) _this.size.y;

	for(int i = 0; i < 8; i++)
	{
		result += avgPixel[i];
	}

	// int vari = pcgRandom(++result) * SAMPLE_IMAGE_COUNT;

	// result += vari;

	// result = ((float)position.x + (float)position.y * (float)_this.size.x)/((float)_this.size.x * (float)_this.size.y);
	// result = ((float)position.x + (float)position.y * (float)_this.size.x) * ratio;
	// result = normalizedPosition.x + normalizedPosition.y * (float)_this.size.x;
	// result = normalizedPosition.x + normalizedPosition.y;

	return result;
}

int compareDatasets(const void *a, const void *b)
{
	DataSet* dsA = (DataSet*) a;
	DataSet* dsB = (DataSet*) b;
	return dsA->hash - dsB->hash;
}

URColor getNearestColor(double hash, DataSet array[], uint32_t numElementos) 
{
	int targetCenter = numElementos / 2;


	DataSet middleHashInfo = array[targetCenter];

	// search for the exact hash.
	while(hash < middleHashInfo.hash && targetCenter != 0)
	{
		targetCenter /= 2;
		middleHashInfo = array[targetCenter];
	}

    for(int i = targetCenter; i < numElementos; i++)
	{
		if(array[i].hash >= hash)
			return array[i].color;
	}
	return array[(int)pcgRandom(hash) * SAMPLE_IMAGE_COUNT].color;
}

void doGenerativeStep(GameState* _this)
{
	int x, y;
	for(y = 0; y < _this->currentFrame.size.y; y++)
	{
		for(x = 0; x < _this->currentFrame.size.x; x++)
		{
			double hash = calculatePixelHash(_this->currentFrame, (URPointI){x, y}, x == 0 && y == 0);
			URColor color = getNearestColor(hash, _this->dataSets, SAMPLE_SIZE);
			urSpritePutPixel(_this->nextFrame, (URPointI){.x = x, .y = y}, color);
		}
	}
}

GameState generateImageLoop(GameState _this)
{
	static int iteration = 0;
	printf("Iteracion %d\n", iteration);
	iteration++;
	graphicsClear(_this.graphics.imageData);

	urSpriteDraw(_this.currentFrame);

	URColor static textColor = {255, 255, 255};
	char *clickToStart[400] = {0};

	snprintf(clickToStart, 400, "iteration number: %d   ", iteration);

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

	static bool justPressed = false;
	
	
	urPrintString((URPointI){UR_SCREEN_WIDTH / 2, UR_SCREEN_HEIGHT / 2}, clickToStart, textColor);
	// if (glfwGetMouseButton(_this.graphics.window, GLFW_MOUSE_BUTTON_1) && !justPressed)
	// {
		doGenerativeStep(&_this);

		URSprite pivot = _this.currentFrame;
		_this.currentFrame = _this.nextFrame;
		_this.nextFrame = _this.currentFrame;

	// 	justPressed = true;
	// } 
	// else if(!glfwGetMouseButton(_this.graphics.window, GLFW_MOUSE_BUTTON_1) && justPressed)
	// {
	// 	justPressed = false;
	// }
	
	return _this;
}

int trainModel(DataSet* dataSets, URSprite sprite)
{
	static uint32_t count = 0;
	for(int y = 0; y < sprite.size.y; y++)
	{
		for(int x = 0; x < sprite.size.x; x++)
		{
			dataSets[count++] = (DataSet){urSpriteGetPixel(sprite, (URPointI){x, y}), calculatePixelHash(sprite, (URPointI){.x = x, .y = y}, y == 0 && x == 0)};
		}
	}
	qsort(dataSets, count, sizeof(DataSet), compareDatasets);

	return count;
}

GameState gameMainLoop(GameState gameState)
{
	imageData = (char *)gameState.graphics.imageData.data;
	gameState.deltaTime = getDeltaTime();
	gameState = gameStateCheckExitConditions(gameState);
	switch (gameState.gameStateEnum)
	{
	case GAME_STATE_CLICK_TO_START:
		gameState = generateImageLoop(gameState);
		break;
	
	default:
		gameState.shouldQuit = true;
	}
	swapBuffersPrintFPSPollEvents(gameState.graphics, gameState.deltaTime);
	if (gameState.shouldStop)
	{
		gameState.shouldStop = false;
		gameState.gameStateEnum++;
	}

	return gameState;
}

int main(void)
{
	// Init memory, load assets and general initialization
	staticAllocatorInit(2878340 * 5);
	initDeltaTime();

#ifndef __EMSCRIPTEN__
	GameState gameState = {0};
#endif

	//gameState.dataSets = allocStatic(SAMPLE_SIZE * sizeof(DataSet));

	gameState._self = &gameState;
	gameState.graphics = graphicsCreate(UR_SCREEN_WIDTH, UR_SCREEN_HEIGHT, false, 1);
	loadAssets(gameState.sprites);
	gameState.currentFrame = gameState.sprites[ASSET_NOISE_A];
	gameState.nextFrame = gameState.sprites[ASSET_NOISE_B];
	// for(int i = ASSET_MODEL_FACE01; i < (ASSET_MODEL_FACE01 + SAMPLE_IMAGE_COUNT); i++)
	// {
	// 	trainModel(gameState.dataSets, gameState.sprites[i]);
	// }
	trainModel(gameState.dataSets, gameState.sprites[ASSET_MODEL_CHECKER]);


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
	staticAllocatorDestroy();
	return 0;
#endif
}
