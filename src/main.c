#include "program/core/utils/memory.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <soloud_c.h>
#define INITIAL_LEVEL GAME_STATE_EDIT_TEXT
#define UR_SCREEN_WIDTH 320
#define UR_SCREEN_HEIGHT 240
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>
#define igGetIO igGetIO_Nil

#define UR_MALLOC MALLOC
#define UR_REALLOC REALLOC
#define UR_FREE FREE

void urPutPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

#define UR_PUT_PIXEL urPutPixel
#include "universal_renderer.h"

#define Color URColor
#define PointI URPointI
#define PointF URPointF

#include "program/core/utils/utils.h"
#include "program/core/input/keyboard.h"

ImageData globalImgData;

void urPutPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	int position = (x + y * UR_SCREEN_WIDTH) % globalImgData.bufferSize;
	globalImgData.data[position] = (URColor){r, g, b};
}

#define S3L_RESOLUTION_X 320
#define S3L_RESOLUTION_Y 240
#define S3L_PIXEL_FUNCTION drawPixel
#include <small3Dlib/small3dlib.h>

void drawPixel(S3L_PixelInfo *p)
{
	URColor color = {0};
	switch (p->triangleID)
	{
	case 1:
	case 0:
		color.r = 0xff;
		break;
	case 2:
	case 3:
		color.b = 0xff;
		break;
	case 6:
	case 7:
		color.g = 0xff;
		break;
	default:
		color.r = color.g = color.b = 0x66;
	}

	urPutPixel(p->x, p->y, color.r, color.g, color.b);
}

#define TILES_CAPACITY 100

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#define ARRAY_MALLOC MALLOC
#define ARRAY_REALLOC REALLOC
#define __ARRAY_T_IMPLEMENTATION__
#include "program/core/array_t/array_t.h"

#define STBI_MALLOC(sz) MALLOC(sz)
#define STBI_REALLOC(p, newsz) REALLOC(p, newsz)
#define STBI_FREE(p) FREE(p)

#define ENEMY_DOWN_OFFSET 53
#define ENEMY_UP_OFFSET 20

#include <soloud_c.h>

typedef enum
{
	SFX_SELECT,
	SFX_SHOOT_HERO,
	SFX_ENEMY_ESCAPED,
	SFX_BLIP,
	SFX_HERO_JUMP,
	SFX_HERO_HURT,
	SFX_COUNT
} GameSFX;

typedef enum
{
	SPEECH_JUMP_THE_ROCKS,
	SPEECH_SHOOT_THE_BAD_GUYS,
	SPEECH_NOOO,
	SPEECH_GAME_OVER,
	SPEECH_COUNT
} GameSpeech;

typedef struct
{
	Soloud *soloud;
	Speech *speechs[SPEECH_COUNT];
	Sfxr *sfx[SFX_COUNT];
} Sound;

typedef enum
{
	ASSET_NONE,
	ASSET_BACKGROUND,
	ASSET_FOREGROUND,
	ASSET_TOP_SCORE_SQUARE,
	ASSET_SIGHT,
	ASSET_SHOOT,

	ASSET_HOW_TO_PLAY,

	ASSET_ENEMY_GREEN_BIG,
	ASSET_ENEMY_GREEN_MEDIUM,
	ASSET_ENEMY_GREEN_SMALL,

	ASSET_ENEMY_GREEN_BIG_SHOOT,
	ASSET_ENEMY_GREEN_MEDIUM_SHOOT,
	ASSET_ENEMY_GREEN_SMALL_SHOOT,

	ASSET_LEVEL2_HERO_GREEN,
	ASSET_LEVEL2_HERO_BLUE,
	ASSET_LEVEL2_HERO_RED,
	ASSET_LEVEL2_HERO_YELLOW,
	ASSET_LEVEL2_BACKGROUND,

	ASSET_LEVEL2_CLOUD_1,
	ASSET_LEVEL2_CLOUD_2,
	ASSET_LEVEL2_CLOUD_3,
	ASSET_LEVEL2_CLOUD_4,

	ASSET_LEVEL2_OBSTACLE_1,
	ASSET_LEVEL2_OBSTACLE_2,
	ASSET_LEVEL2_OBSTACLE_3,

	ASSET_LEVEL2_HOW_TO_PLAY,

	ASSET_LEVEL3_BACKGROUND,
	ASSET_LEVEL3_BACKGROUND_TILED,
	ASSET_LEVEL3_HERO_RUN,
	ASSET_LEVEL3_HERO_JUMP,
	ASSET_LEVEL3_HERO_FALL,
	ASSET_LEVEL3_HERO_IDLE,
	ASSET_LEVEL3_ENEMY_WALKING,
	ASSET_LEVEL3_ENEMY_HIT,
	ASSET_LEVEL3_ENEMY_SINGLE_FRAME,

	ASSET_COUNT
} Assets;

typedef enum
{
	ENEMY_STATE_HIDDEN,
	ENEMY_STATE_GOING_UP,
	ENEMY_STATE_GOING_DOWN,
	ENEMY_STATE_DEAD,
	ENEMY_STATE_COUNT
} EnemyState;

typedef struct
{
	EnemyState state;
	int spriteId;
	int lowerClippingPosition;
	int topLimit;
	int bottomOffset;
	bool visible;
	float elapsedStateTime;
	float speedMultiplicator;
	PointI basePosition;
	PointF position;
} Enemy;

typedef enum
{
	QPOS_TOP_LEFT,
	QPOS_TOP,
	QPOS_TOP_RIGHT,
	QPOS_LEFT,
	QPOS_RIGHT,
	QPOS_BOTTOM_LEFT,
	QPOS_BOTTOM,
	QPOS_BOTTOM_RIGHT,
	QPOS_NONE,
	QPOS_COUNT
} QuadrantPosition;

struct GameState;

#define ENEMIES_CAPACITY 2

#define PARTICLES_CAPACITY (34 * 34)

typedef enum
{
	GAME_STATE_CLICK_TO_START,
	GAME_STATE_EDIT_TEXT,	

	GAME_STATE_COUNT
} GameStateEnum;

typedef struct GameState
{
	struct GameState *_self;
	Graphics graphics;
	ImGuiIO *ioptr;
	URSprite sprites[ASSET_COUNT];
	Sound sound;
	float deltaTime;
	bool shouldQuit;
	bool shouldStop;
	bool showDemoWindow;
	GameStateEnum gameStateEnum;
	char* buffer;
	array_t text_lines;
	URPointI cursor_position;
	int highest_cursor_x;
 	bool should_draw_cursor;
	int rows;
	int cols;
} GameState;

GameState gameStateCheckExitConditions(GameState _this)
{
	if (glfwWindowShouldClose(_this.graphics.window))
	{
		_this.shouldQuit = true;
		return _this;
	}

	_this.shouldQuit = isKeyJustPressed(_this.graphics.window, GLFW_KEY_ESCAPE);

	return _this;
}

void swapBuffersPrintFPSPollEvents(Graphics graphics, float deltaTime)
{
	urPrintFPS(deltaTime);
	graphicsSwapBuffers(graphics);
	glfwPollEvents();
}

Sound sound_create()
{
	Sound that = {0};

	that.soloud = Soloud_create();
	for (int i = 0; i < SPEECH_COUNT; i++)
	{
		that.speechs[i] = Speech_create();
	}

	for (int i = 0; i < SFX_COUNT; i++)
	{
		that.sfx[i] = Sfxr_create();
	}

	Speech_setText(that.speechs[SPEECH_JUMP_THE_ROCKS], "Jump the rocks!");
	Speech_setText(that.speechs[SPEECH_SHOOT_THE_BAD_GUYS], "Shoot the bad guys!");
	Speech_setText(that.speechs[SPEECH_NOOO], "O!");
	Sfxr_loadPreset(that.sfx[SFX_BLIP], SFXR_BLIP, 3247);
	Sfxr_loadPreset(that.sfx[SFX_SELECT], SFXR_POWERUP, 3247);
	Sfxr_loadPreset(that.sfx[SFX_SHOOT_HERO], SFXR_EXPLOSION, 3247);
	Sfxr_loadPreset(that.sfx[SFX_ENEMY_ESCAPED], SFXR_HURT, 3247);
	Sfxr_loadPreset(that.sfx[SFX_HERO_JUMP], SFXR_JUMP, 3247);
	Sfxr_loadPreset(that.sfx[SFX_HERO_HURT], SFXR_HURT, 3247);

	Soloud_initEx(
		that.soloud,
		SOLOUD_CLIP_ROUNDOFF | SOLOUD_ENABLE_VISUALIZATION,
		SOLOUD_AUTO, SOLOUD_AUTO, SOLOUD_AUTO, 2);

	Soloud_setGlobalVolume(that.soloud, 4);

	return that;
}

void soundPlaySfx(Sound that, GameSFX id)
{
	Soloud_play(that.soloud, that.sfx[id]);
}

void soundPlaySpeech(Sound that, GameSpeech id)
{
	Soloud_play(that.soloud, that.speechs[id]);
}

void soundDestroy(Sound that)
{
	for (int i = 0; i < SPEECH_COUNT; i++)
	{
		Speech_destroy(that.speechs[i]);
	}

	for (int i = 0; i < SFX_COUNT; i++)
	{
		Sfxr_destroy(that.sfx[i]);
	}

	Soloud_deinit(that.soloud);
	Soloud_destroy(that.soloud);
}

void loadAssets(URSprite *_this)
{
	_this[ASSET_BACKGROUND] = urSpriteCreate("assets/background.bmp");
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

GameState renderDearImgui(GameState gameState)
{
	// render
	// start imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	if (igBeginMainMenuBar())
    {
        if (igBeginMenu("Edit", true))
        {
            if (igMenuItem_Bool("Undo", "CTRL+Z", true, true)) {}
            if (igMenuItem_Bool("Redo", "CTRL+Y", false, false)) {} // Disabled item
            igSeparator();
            if (igMenuItem_Bool("Cut", "CTRL+X", true, true)) {}
            if (igMenuItem_Bool("Copy", "CTRL+C", true, true)) {}
            if (igMenuItem_Bool("Paste", "CTRL+V", true, true)) {}
            igEndMenu();
        }
        igEndMainMenuBar();
    }
	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
#ifdef IMGUI_HAS_DOCK
	if (ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow *backup_current_window = glfwGetCurrentContext();
		igUpdatePlatformWindows();
		igRenderPlatformWindowsDefault(NULL, NULL);
		glfwMakeContextCurrent(backup_current_window);
	}
#endif
	return gameState;
}

void *read_file(char* fileName)
{
	FILE *fp = fopen("assets/test.c", "rb");
	
	if(!fp)
	{
		printf("file not found\n");
		fclose(fp);
		return NULL;
	} else {
		printf("file found\n");
	}

	if(fseek(fp, 0, SEEK_END) != 0)
	{
		printf("fseek failed\n");
		fclose(fp);
		return NULL;
	}

	long file_size = ftell(fp);
	if(file_size < 0)
	{
		printf("fseek failed\n");
		fclose(fp);
		return NULL;
	}

	char* buffer = malloc((size_t)file_size + 1);

	rewind(fp);

	size_t bytes_read = fread(buffer, 1, (size_t)file_size, fp);

	if(bytes_read != (size_t)file_size)
	{
		printf("file read failed\n");
		fclose(fp);
		return NULL;
	}

	buffer[file_size] = '\0';
	fclose(fp);
	return buffer;
}

void urPrintStringWithSytaxHighlight(URPointI topLeftCorner, char *string, URColor color)
{
	URColor current_color = UR_WHITE;
	static URColor text_color = UR_WHITE;
	static URColor include_color = UR_RED;
	static URColor bracket_color = {0, 200, 190};
	static URColor punctuation_color = {200, 200, 0};
	static URColor shadow_color = {0, 0, 0};
	static URColor string_color = UR_GREEN;
	bool string_in_progress  = false;
	bool word_in_progress = false;

	size_t stringLen = strlen(string);

	URPointI original_position = topLeftCorner;

	URPointI current_position = topLeftCorner;

	for (size_t i = 0; i < stringLen; i++)
	{
		if (string[i] == '\t')
		{
			current_position.x += 6 * 4;
		} else if (string[i] == '\n')
		{
			current_color = color;
			current_position.x = original_position.x;
			current_position.y += 6;
		} else if (string[i] == ' ')
		{
			current_position.x += 6;
		} 

		if (string[i] == '.')
		{
			int charOffset = 63;
			urDrawCharacterWithShadow(current_position, charOffset, punctuation_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == '%')
		{
			int charOffset = 62;
			urDrawCharacterWithShadow(current_position, charOffset, punctuation_color, shadow_color);
			current_position.x += 6;
		}
		
		if (string[i] == '#')
		{
			int charOffset = 64;
			urDrawCharacterWithShadow(current_position, charOffset, include_color, shadow_color);
			current_color = include_color;
			current_position.x += 6;
		}

		if (string[i] == '<')
		{
			int charOffset = 65;
			urDrawCharacterWithShadow(current_position, charOffset, bracket_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == '>')
		{
			int charOffset = 66;
			urDrawCharacterWithShadow(current_position, charOffset, bracket_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == ',')
		{
			int charOffset = 67;
			urDrawCharacterWithShadow(current_position, charOffset, current_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == '(' )
		{
			int charOffset = 68;
			urDrawCharacterWithShadow(current_position, charOffset, bracket_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == ')' )
		{
			int charOffset = 69;
			urDrawCharacterWithShadow(current_position, charOffset, bracket_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == '{' )
		{
			int charOffset = 70;
			urDrawCharacterWithShadow(current_position, charOffset, bracket_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == '}' )
		{
			int charOffset = 71;
			urDrawCharacterWithShadow(current_position, charOffset, bracket_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == '*' )
		{
			int charOffset = 72;
			urDrawCharacterWithShadow(current_position, charOffset, current_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] == '"' )
		{
			int charOffset = 73;
			current_color = string_color;
			urDrawCharacterWithShadow(current_position, charOffset, current_color, shadow_color);
			if(string_in_progress)
			{
				string_in_progress = false;
				current_color = text_color;
				current_position.x += 6;
				continue;
			}
			string_in_progress = true;
			current_position.x += 6;
		}

		if (string[i] == ';' )
		{
			int charOffset = 74;
			urDrawCharacterWithShadow(current_position, charOffset, punctuation_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] >= '0' && string[i] <= '9')
		{
			urDrawCharacterWithShadow(current_position, string[i] - '0', current_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] >= 'a' && string[i] <= 'z')
		{
			int charOffset = string[i] - 'a' + 36;
			urDrawCharacterWithShadow(current_position, charOffset, current_color, shadow_color);
			current_position.x += 6;
		}

		if (string[i] >= 'A' && string[i] <= 'Z')
		{
			int charOffset = string[i] + 32 - 'a' + 10;
			urDrawCharacterWithShadow(current_position, charOffset, current_color, shadow_color);
			current_position.x += 6;
		}
	}
}

void draw_cursor(URPointI cursor_position, URColor cursor_color)
{
	URPointI start, end;
	start.x = 2 + cursor_position.x * 6;
	start.y = (cursor_position.y + 1) * 6;
	end.x = start.x + 4;
	end.y = start.y;
	urDrawLine(start, end, cursor_color);
}

array_t* get_current_line(GameState _that)
{
	array_t lines = _that.text_lines;
	array_t* line = (array_t*)lines.get_element_at(lines, _that.cursor_position.y);
	return line;
}

GameState handle_cursor_position(GameState _that)
{
	static float elapsed_time = 0;
	elapsed_time += _that.deltaTime;
	array_t* line = get_current_line(_that);

	if(elapsed_time < 0.1)
	{
		return;
	}

	elapsed_time = 0;

	if(glfwGetKey(_that.graphics.window, GLFW_KEY_RIGHT))
	{
		_that.cursor_position.x++;
		_that.should_draw_cursor = true;
		if(_that.cursor_position.x > line->length)
		{
			_that.cursor_position.y++;
			_that.cursor_position.x = 0;
		}
		_that.highest_cursor_x = _that.cursor_position.x;
	}

	if(glfwGetKey(_that.graphics.window, GLFW_KEY_LEFT))
	{
		_that.cursor_position.x--;
		_that.should_draw_cursor = true;
		_that.highest_cursor_x = _that.cursor_position.x;
		if(_that.cursor_position.x < 0 && _that.cursor_position.y > 0)
		{
			_that.cursor_position.y--;
			line = get_current_line(_that);
			_that.cursor_position.x = line->length;
		}
		_that.highest_cursor_x = _that.cursor_position.x;
	}

	if(glfwGetKey(_that.graphics.window, GLFW_KEY_DOWN) || glfwGetKey(_that.graphics.window, GLFW_KEY_ENTER))
	{
		_that.cursor_position.y++;
		_that.should_draw_cursor = true;
	}

	if(glfwGetKey(_that.graphics.window, GLFW_KEY_UP))
	{
		_that.cursor_position.y--;
		_that.should_draw_cursor = true;
	}

	line = get_current_line(_that);
	_that.cursor_position.x = MIN(line->length, _that.cursor_position.x);
	return _that;
}
char last_key_pressed = 0;

GameState process_state_edit_text(GameState _that)
{
	URColor static background_color = {50, 70, 110};
	URColor static textColor = {255, 255, 255};
	static bool draw_cursor_b = false;
	
	if(last_key_pressed != 0)
	{
		soundPlaySfx(_that.sound, SFX_HERO_HURT);
		printf("last key pressed %c\n", last_key_pressed);
		array_t* line = get_current_line(_that);
		
		if(_that.cursor_position.x < line->length)
		{
			line->insert_element_at(line, &last_key_pressed, _that.cursor_position.x);
		} else {
			line->append_element(line, &last_key_pressed);
		}
	
		_that.cursor_position.x++;
		last_key_pressed = 0;
	}
	
	static float elapsedTime = 0;
	elapsedTime += _that.deltaTime;
	if(elapsedTime > 0.3)
	{
		draw_cursor_b = !draw_cursor_b;
		elapsedTime = 0;
	} 
	
	_that = handle_cursor_position(_that);

	graphicsClearColor(_that.graphics.imageData, background_color);
	
	for (int i = 0; i < _that.text_lines.length; i++) {
 	    array_t* line = _that.text_lines.get_element_at(_that.text_lines, i);
	    char* str = (char*)line->data;
		urPrintStringWithSytaxHighlight((URPointI){1, i*6 + 1}, str, textColor);
	}

	if(draw_cursor_b)
	{
		draw_cursor(_that.cursor_position, UR_WHITE);
	}
	
	return _that;
}

GameState game_main_loop(GameState gameState)
{
	globalImgData = gameState.graphics.imageData;
	gameState.deltaTime = getDeltaTime();
	gameState = gameStateCheckExitConditions(gameState);

	switch (gameState.gameStateEnum)
	{
	case GAME_STATE_CLICK_TO_START:
		gameState = gameStateClickToStart(gameState);
		break;
	case GAME_STATE_EDIT_TEXT:
		gameState = process_state_edit_text(gameState);
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
	// gameState = renderDearImgui(gameState);
	glfwSwapBuffers(gameState.graphics.window);

	return gameState;
}

#ifdef __EMSCRIPTEN__
GameState gameState = {0};

void emscriptenLoopHandler()
{
	gameState = game_main_loop(gameState);
}
#endif

GameState initDearImgui(GameState gameState)
{
	// setup imgui
	igCreateContext(NULL);

	// set docking
	gameState.ioptr = igGetIO();
	gameState.ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
																		// ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
#ifdef IMGUI_HAS_DOCK
	ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;	// Enable Docking
	ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
#endif

// Shader version
#ifndef __EMSCRIPTEN__
	const char *glsl_version = "#version 130";
#else
	const char *glsl_version = "#version 300 es";
#endif


	ImGui_ImplGlfw_InitForOpenGL(gameState.graphics.window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	igStyleColorsDark(NULL);

	gameState.showDemoWindow = true;
	bool showAnotherWindow = false;
	return gameState;
}

void keyboard_callback(GLFWwindow* window, unsigned int key)
{
	if(key < 128)
	{
		last_key_pressed = key;
	}
}

GameState game_state_create()
{
	GameState gameState = {0};
	gameState.graphics = graphicsCreate(320, 240, false);
	gameState.rows = UR_SCREEN_HEIGHT / 7;
	gameState.cols = UR_SCREEN_WIDTH / 7;

	gameState.text_lines = array_create(gameState.cols, sizeof(gameState.text_lines));
	
	for (int i = 0; i < gameState.cols; i++)
	{
		array_t line = array_create(gameState.rows, sizeof(char));
		gameState.text_lines.append_element(&gameState.text_lines, &line);
	}

	gameState.sound = sound_create();

	gameState.buffer = read_file("assets/test.c");
	char* cursor = gameState.buffer;

	int row = 0;
	while (*cursor)
	{
		while (*cursor && *cursor != '\n')
		{
			array_t* current_line = gameState.text_lines.get_element_at(gameState.text_lines, row);
			if(*cursor != '\n')
			{
				current_line->append_element(current_line, cursor);
			}
			cursor++;
		}
		if (*cursor == '\n') cursor++;
		row++;
	}

	return gameState;
}

int main(void)
{
	initDeltaTime();

	GameState gameState = game_state_create();
	gameState._self = &gameState;

	glfwSetCharCallback(gameState.graphics.window, keyboard_callback);

	gameState = initDearImgui(gameState);

#ifdef INITIAL_LEVEL
	gameState.gameStateEnum = INITIAL_LEVEL;
#endif

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(emscriptenLoopHandler, 0, false);
#else


	while (!gameState.shouldStop && !gameState.shouldQuit)
		gameState = game_main_loop(gameState);

Cleanup:
	graphicsDestroy(gameState.graphics);
	return 0;
#endif
}
