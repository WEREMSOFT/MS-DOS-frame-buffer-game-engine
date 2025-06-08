#include "program/core/utils/memory.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#define INITIAL_LEVEL GAME_STATE_EDIT_TEXT
#define UR_SCREEN_WIDTH 320
#define UR_SCREEN_HEIGHT 240

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
	if(!(x >= 0 && y >= 0 && x < UR_SCREEN_WIDTH && y < UR_SCREEN_HEIGHT))
		return;

	int position = (x + y * UR_SCREEN_WIDTH) % globalImgData.bufferSize;
	globalImgData.data[position] = (URColor){r, g, b};
}

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



struct game_state_t;

#define ENEMIES_CAPACITY 2

#define PARTICLES_CAPACITY (34 * 34)

typedef enum
{
	GAME_STATE_CLICK_TO_START,
	GAME_STATE_EDIT_TEXT,	

	GAME_STATE_COUNT
} GameStateEnum;

typedef struct game_state_t
{
	struct game_state_t *_self;
	Graphics graphics;
	float deltaTime;
	bool shouldQuit;
	bool shouldStop;
	bool showDemoWindow;
	GameStateEnum gameStateEnum;
	char* buffer;
	array_t document;
	URPointI cursor_position;
	int highest_cursor_x;
 	bool should_draw_cursor;
	int rows;
	int cols;
	int starting_row;
	int tab_size;
} game_state_t;

game_state_t gameStateCheckExitConditions(game_state_t _this)
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

game_state_t gameStateClickToStart(game_state_t _this)
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

array_t* get_current_line(game_state_t _that)
{
	array_t lines = _that.document;
	array_t* line = (array_t*)lines.get_element_at(lines, _that.cursor_position.y);
	return line;
}

int last_key_pressed = 0;

game_state_t handle_cursor_position(game_state_t _that)
{
	array_t* line = get_current_line(_that);
	
	if(last_key_pressed == GLFW_KEY_HOME)
	{
		_that.cursor_position.x = 0;
	}

	if(last_key_pressed == GLFW_KEY_END)
	{
		_that.cursor_position.x = line->length;
	}

	if(last_key_pressed == GLFW_KEY_RIGHT)
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

	if(last_key_pressed == GLFW_KEY_LEFT)
	{
		if(_that.cursor_position.x == 0 && _that.cursor_position.y == 0) return;

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

	if(last_key_pressed == GLFW_KEY_DOWN)
	{
		_that.cursor_position.y++;
		
		_that.cursor_position.y = MIN(_that.document.length - 1, _that.cursor_position.y);

		if(_that.cursor_position.y - _that.starting_row > _that.rows)
		{
			_that.starting_row++;
		}
		_that.should_draw_cursor = true;
	}

	if(last_key_pressed == GLFW_KEY_PAGE_DOWN)
	{
		_that.cursor_position.y += _that.rows;
		_that.starting_row += _that.rows;

		_that.cursor_position.y = MIN(_that.document.length - 1, _that.cursor_position.y);
		_that.starting_row = MIN(_that.document.length - 1, _that.document.length - _that.rows);
		
		_that.should_draw_cursor = true;
	}

	if(last_key_pressed == GLFW_KEY_ENTER)
	{
		_that.cursor_position.y++;
		_that.cursor_position.x = 0;
		_that.should_draw_cursor = true;
	}

	if(last_key_pressed == GLFW_KEY_UP)
	{
		if(_that.cursor_position.y - _that.starting_row == 0)
		{
			_that.starting_row = MAX(0, _that.starting_row - 1);;
		}
		_that.cursor_position.y = MAX(0, _that.cursor_position.y - 1);

		_that.should_draw_cursor = true;
	}

	if(last_key_pressed == GLFW_KEY_PAGE_UP)
	{
		_that.cursor_position.y -= _that.rows;
		_that.starting_row -= _that.rows;

		_that.starting_row = MAX(0, _that.starting_row);
		_that.cursor_position.y = MAX(0, _that.cursor_position.y);

		_that.should_draw_cursor = true;
	}

	line = get_current_line(_that);
	_that.highest_cursor_x = MAX(_that.cursor_position.x, _that.highest_cursor_x);
	if(line->length < _that.cursor_position.x)
	{
		_that.cursor_position.x = MIN(line->length, _that.cursor_position.x);
	} 
	else 
	{
		if(line->length >= _that.highest_cursor_x)
		{
			_that.cursor_position.x = _that.highest_cursor_x;
		}
	}

	return _that;
}

game_state_t handle_editor_keyboard_backspace(game_state_t _that)
{
	array_t* line = get_current_line(_that);
	if(last_key_pressed != GLFW_KEY_BACKSPACE)
	{
		return _that;
	}

	if(_that.cursor_position.x == 0)
	{
		if(_that.cursor_position.y == 0)
		{
			return _that;
		}

		array_t* prev_line = _that.document.get_element_at(_that.document, _that.cursor_position.y - 1);
		int prev_line_length = prev_line->length;
		prev_line->concatenate(prev_line, *line);
		_that.document.delete_element_at(&_that.document, _that.cursor_position.y);
		_that.cursor_position.x = prev_line_length;
		_that.cursor_position.y --;
		last_key_pressed = 0;
		return _that;
	}
	
	line->delete_element_at(line, _that.cursor_position.x - 1);
	_that.cursor_position.x--;
	_that.highest_cursor_x = _that.cursor_position.x;
	return _that;
}

game_state_t handle_editor_keyboard(game_state_t _that)
{
	if(last_key_pressed != 0)
	{
		if(last_key_pressed < 128)
		{
			array_t* line = get_current_line(_that);

			if(_that.cursor_position.x < line->length)
			{
				line->insert_element_at(line, &last_key_pressed, _that.cursor_position.x);
			} else {
				line->append_element(line, &last_key_pressed);
			}
			_that.cursor_position.x++;
		}
	}

	_that = handle_editor_keyboard_backspace(_that);

	if(last_key_pressed == GLFW_KEY_DELETE)
	{
		array_t* line = get_current_line(_that);
		line->delete_element_at(line, _that.cursor_position.x);			
	}

	if(last_key_pressed == GLFW_KEY_ENTER)
	{
		array_t* line = get_current_line(_that);
		array_t new_line = array_create(_that.rows, sizeof(char));
		new_line.length = strlen(line->data + _that.cursor_position.x);
		strcpy(new_line.data, line->data + _that.cursor_position.x);
		_that.document.insert_element_at(&_that.document, &new_line, _that.cursor_position.y + 1);

		memset(line->data + _that.cursor_position.x, 0, strlen(line->data + _that.cursor_position.x)); 

		line->length = strlen(line->data);
		_that.highest_cursor_x = _that.cursor_position.x = 0;
	}

	_that = handle_cursor_position(_that);

	last_key_pressed = 0;
	
	return _that;
}

game_state_t process_state_edit_text(game_state_t _that)
{
	URColor static background_color = {50, 70, 110};
	URColor static textColor = {255, 255, 255};
	static bool draw_cursor_b = false;
	
	static float elapsedTime = 0;
	elapsedTime += _that.deltaTime;
	if(elapsedTime > 0.3)
	{
		draw_cursor_b = !draw_cursor_b;
		elapsedTime = 0;
	} 
	
	_that = handle_editor_keyboard(_that);

	graphicsClearColor(_that.graphics.imageData, background_color);
	
	int line_number = 0;
	for (int i = _that.starting_row; i < _that.document.length && i < _that.starting_row + _that.rows; i++) {
 	    array_t* line = _that.document.get_element_at(_that.document, i);
	    char* str = (char*)line->data;
		urPrintStringWithSytaxHighlight((URPointI){1, line_number*6 + 1}, str, textColor);
		line_number++;
	}

	if(draw_cursor_b)
	{
		URPointI local_cursor_position = _that.cursor_position;
		local_cursor_position.y -= _that.starting_row;
		draw_cursor(local_cursor_position, UR_WHITE);
	}
	
	return _that;
}

game_state_t game_main_loop(game_state_t gameState)
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

void keyboard_char_callback(GLFWwindow* window, unsigned int key)
{
	if(key < 128)
	{
		last_key_pressed = key;
	}
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if((key == GLFW_KEY_UP ||
		key == GLFW_KEY_DOWN ||
		key == GLFW_KEY_LEFT ||
		key == GLFW_KEY_RIGHT ||
		key == GLFW_KEY_BACKSPACE ||
		key == GLFW_KEY_ENTER ||
		key == GLFW_KEY_HOME ||
		key == GLFW_KEY_END ||
		key == GLFW_KEY_PAGE_DOWN ||
		key == GLFW_KEY_PAGE_UP ||
		key == GLFW_KEY_DELETE)
		&& action == GLFW_PRESS && key > 128)
	{
		last_key_pressed = key;
	}
}

game_state_t game_state_create()
{
	game_state_t gameState = {0};
	gameState.starting_row = 0;
	gameState.graphics = graphicsCreate(UR_SCREEN_WIDTH, UR_SCREEN_HEIGHT, false);
	gameState.rows = UR_SCREEN_HEIGHT / 7;
	gameState.cols = UR_SCREEN_WIDTH / 7;
	gameState.tab_size = 4;

	gameState.document = array_create(gameState.cols, sizeof(gameState.document));
	
	for (int i = 0; i < gameState.cols; i++)
	{
		array_t line = array_create(gameState.rows, sizeof(char));
		gameState.document.append_element(&gameState.document, &line);
	}

	gameState.buffer = read_file("assets/test.c");
	char* cursor = gameState.buffer;

	int row = 0;
	while (*cursor)
	{
		while (*cursor && *cursor != '\n')
		{
			array_t* current_line = gameState.document.get_element_at(gameState.document, row);

			while(current_line == NULL)
			{
				array_t line = array_create(gameState.rows, sizeof(char));
				gameState.document.append_element(&gameState.document, &line);
				current_line = gameState.document.get_element_at(gameState.document, row);
			}

			if(*cursor == '\t')
			{
				for(int i = 0; i < gameState.tab_size; i++)
				{
					char space = ' ';
					current_line->append_element(current_line, &space);	
				}
			} else if(*cursor != '\n')
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

	game_state_t gameState = game_state_create();
	gameState._self = &gameState;

	glfwSetCharCallback(gameState.graphics.window, keyboard_char_callback);
	glfwSetKeyCallback(gameState.graphics.window, keyboard_callback);

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
