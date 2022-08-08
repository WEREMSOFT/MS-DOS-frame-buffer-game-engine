#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "program/core/utils/utils.h"
#include "program/core/graphics/graphics.h"
#include "program/core/sprite/sprite.h"
#include "program/sound/sound.h"

#define __STATIC_ALLOC_IMPLEMENTATION__
#include "program/core/stackAllocator/staticAlloc.h"

#define ARRAY_MALLOC allocStatic
#define ARRAY_REALLOC reallocStatic
#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "program/core/array/array.h"

#define STBI_MALLOC(sz) allocStatic(sz)
#define STBI_REALLOC(p, newsz) reallocStatic(p, newsz)
#define STBI_FREE(p) freeStatic(p)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define ENEMY_SPEED 50.
#define ENEMY_DOWN_OFFSET 53
#define ENEMY_UP_OFFSET 20

PointI ENEMY_OFFSET;

typedef enum
{
    ASSET_BACKGROUND,
    ASSET_SIGHT,
    ASSET_SHOOT,
    ASSET_ENEMY_GREEN_BIG,
    ASSET_ENEMY_GREEN_BIG_SHOOT,
    ASSET_ENEMY_GREEN_MEDIUM,
    ASSET_ENEMY_GREEN_MEDIUM_SHOOT,
    ASSET_ENEMY_GREEN_SMALL,
    ASSET_ENEMY_GREEN_SMALL_SHOOT,
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
    int downClip;
    int topOffset;
    int bottomOffset;
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

typedef struct
{
    Graphics graphics;
    Sprite sprites[ASSET_COUNT];
    PointI textPosition;
    QuadrantPosition quadPosition;
    Sound sound;
    bool shouldQuit;
    Enemy enemies[8];
    PointI positions[QPOS_COUNT];
} Level1;

typedef struct
{
    Graphics graphics;
    Level1 mainMenu;
    Sprite sprites[ASSET_COUNT];
    Sound sound;
} Program;

void spritesLoad(Sprite *this)
{
    this[ASSET_BACKGROUND] = spriteCreate("assets/320x240Test.png");
    this[ASSET_SIGHT] = spriteCreate("assets/aimcross.png");

    this[ASSET_SHOOT] = spriteCreate("assets/shoot.bmp");
    this[ASSET_SHOOT].animated = true;
    this[ASSET_SHOOT].animation.frameCount = 4;
    this[ASSET_SHOOT].animation.currentFrame = 0;
    this[ASSET_SHOOT].animation.frameWidth = 55;
    this[ASSET_SHOOT].animation.frameRate = 10.;

    this[ASSET_ENEMY_GREEN_BIG] = spriteCreate("assets/enemyGreenBig2.bmp");
    this[ASSET_ENEMY_GREEN_BIG_SHOOT] = spriteCreate("assets/enemyGreenBig1.bmp");

    this[ASSET_ENEMY_GREEN_MEDIUM] = spriteCreate("assets/enemyGreenSmall2.bmp");
    this[ASSET_ENEMY_GREEN_MEDIUM_SHOOT] = spriteCreate("assets/enemyGreenSmall1.bmp");

    this[ASSET_ENEMY_GREEN_SMALL] = spriteCreate("assets/enemyGreenTiny2.bmp");
    this[ASSET_ENEMY_GREEN_SMALL_SHOOT] = spriteCreate("assets/enemyGreenTiny1.bmp");
}

Enemy enemyPassToStateHidden(Enemy this)
{
    this.state = ENEMY_STATE_HIDDEN;
    this.position.y = this.basePosition.y + ENEMY_DOWN_OFFSET;
    return this;
}

Enemy enemyPassToStateGoingUp(Enemy this)
{
    this.state = ENEMY_STATE_GOING_UP;
    this.position.y = this.basePosition.y + ENEMY_DOWN_OFFSET;
    return this;
}

Enemy enemyPassToStateGoingDown(Enemy this)
{
    this.state = ENEMY_STATE_GOING_DOWN;
    return this;
}

void enemyProcessStateGoingDown(Enemy *enemies, float deltaTime)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_DOWN)
            continue;
        if (enemies[i].position.y > enemies[i].basePosition.y + enemies[i].bottomOffset)
        {
            enemies[i] = enemyPassToStateHidden(enemies[i]);
            continue;
        }
        enemies[i].position.y += ENEMY_SPEED * deltaTime;
    }
}

void enemyProcessStateGoingUp(Enemy *enemies, float deltaTime)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_UP)
            continue;
        if (enemies[i].position.y < enemies[i].basePosition.y - enemies[i].topOffset)
        {
            enemies[i] = enemyPassToStateGoingDown(enemies[i]);
            continue;
        }
        enemies[i].position.y -= ENEMY_SPEED * deltaTime;
    }
}

void level1SetPositions(PointI *positions)
{
    positions[QPOS_TOP_LEFT].x = 42;
    positions[QPOS_TOP_LEFT].y = 55;

    positions[QPOS_TOP].x = 125;
    positions[QPOS_TOP].y = 50;

    positions[QPOS_TOP_RIGHT].x = 260;
    positions[QPOS_TOP_RIGHT].y = 55;

    positions[QPOS_RIGHT].x = 265;
    positions[QPOS_RIGHT].y = 130;

    positions[QPOS_BOTTOM_RIGHT].x = 225;
    positions[QPOS_BOTTOM_RIGHT].y = 180;

    positions[QPOS_BOTTOM].x = 125;
    positions[QPOS_BOTTOM].y = 180;

    positions[QPOS_BOTTOM_LEFT].x = 60;
    positions[QPOS_BOTTOM_LEFT].y = 180;

    positions[QPOS_LEFT].x = 55;
    positions[QPOS_LEFT].y = 125;

    positions[QPOS_TOP_LEFT].x = 42;
    positions[QPOS_TOP_LEFT].y = 55;
}

static Level1 level1InitializeEnemies(Level1 _this)
{
    for (int i = 0; i < 8; i++)
    {
        _this.enemies[i].position.x = _this.enemies[i].basePosition.x = (int)_this.positions[i].x + ENEMY_OFFSET.x;
        _this.enemies[i].position.y = _this.enemies[i].basePosition.y = (int)_this.positions[i].y + ENEMY_OFFSET.y;
        _this.enemies[i] = enemyPassToStateHidden(_this.enemies[i]);
        if (i <= QPOS_TOP_RIGHT)
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_SMALL;
        else if (i >= QPOS_LEFT && i <= QPOS_RIGHT)
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_MEDIUM;
        else
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_BIG;

        _this.enemies[i].lowerClippingPosition = 240;
    }

    _this.enemies[QPOS_LEFT].lowerClippingPosition = 157;
    _this.enemies[QPOS_RIGHT].lowerClippingPosition = 180;
    _this.enemies[QPOS_BOTTOM_LEFT].lowerClippingPosition = 240;
    _this.enemies[QPOS_BOTTOM_RIGHT].lowerClippingPosition = 240;
    _this.enemies[QPOS_BOTTOM].lowerClippingPosition = 240;

    return _this;
}

Level1 level1HandleControls(Level1 _this)
{

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP_LEFT];
        spriteDrawTransparentClipped(_this.sprites[ASSET_SIGHT], _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP_LEFT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP_RIGHT];
        spriteDrawTransparentClipped(_this.sprites[ASSET_SIGHT], _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM_LEFT];
        spriteDrawTransparentClipped(_this.sprites[ASSET_SIGHT], _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM_LEFT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM_RIGHT];
        spriteDrawTransparentClipped(_this.sprites[ASSET_SIGHT], _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP];
        spriteDrawTransparentClipped(_this.sprites[ASSET_SIGHT], _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM];
        spriteDrawTransparentClipped(_this.sprites[ASSET_SIGHT], _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        _this.sprites[ASSET_SIGHT].position = _this.positions[QPOS_RIGHT];
        spriteDrawTransparentClipped(_this.sprites[ASSET_SIGHT], _this.graphics.imageData);
        _this.quadPosition = QPOS_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        _this.sprites[ASSET_SIGHT].position = _this.positions[QPOS_LEFT];
        spriteDrawTransparentClipped(_this.sprites[ASSET_SIGHT], _this.graphics.imageData);
        _this.quadPosition = QPOS_LEFT;
        return _this;
    }

    _this.quadPosition = QPOS_NONE;
    return _this;
}

Level1 level1Create(Graphics graphics, Sprite *sprites, Sound sound)
{
    Level1 _this = {0};

    level1SetPositions(_this.positions);

    _this.sound = sound;
    _this.graphics = graphics;
    for (int i = 0; i < ASSET_COUNT; i++)
    {
        _this.sprites[i] = sprites[i];
    }

    ENEMY_OFFSET.y = 0;
    ENEMY_OFFSET.x = -8;

    _this.shouldQuit = false;

    _this = level1InitializeEnemies(_this);

    for (int i = 0; i < 8; i++)
    {
        _this.enemies[i] = enemyPassToStateHidden(_this.enemies[i]);
    }

    return _this;
}

void level1EnemiesDraw(Level1 _this)
{
    for (int i = 0; i < 8; i++)
    {
        if (_this.enemies[i].state == ENEMY_STATE_HIDDEN)
            continue;
        _this.sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
        _this.sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;
        spriteDrawTransparentClippedLowerLine(_this.sprites[_this.enemies[i].spriteId], _this.graphics.imageData, _this.enemies[i].lowerClippingPosition);
    };
}

Level1 level1Update(Level1 _this)
{
    soundPlaySpeech(_this.sound, SPEECH_SELECT_SHIP);
    bool shouldContinue = true;
    double elapsedTime = glfwGetTime();
    while (shouldContinue && !_this.shouldQuit)
    {
        float dt = getDeltaTime();
        graphicsClear(_this.graphics.imageData);

        _this.shouldQuit = isKeyJustPressed(_this.graphics.window, GLFW_KEY_ESCAPE);
        spriteDrawClipped(_this.sprites[ASSET_BACKGROUND], _this.graphics.imageData);

        if (glfwGetTime() - elapsedTime > 1.)
        {
            elapsedTime = glfwGetTime();
            int enemyToDisplay = rand() % 8;
            if (_this.enemies[enemyToDisplay].state == ENEMY_STATE_HIDDEN)
                _this.enemies[enemyToDisplay] = enemyPassToStateGoingUp(_this.enemies[enemyToDisplay]);
        }

        enemyProcessStateGoingDown(_this.enemies, dt);
        enemyProcessStateGoingUp(_this.enemies, dt);

        level1EnemiesDraw(_this);

        _this = level1HandleControls(_this);

        if (_this.sprites[ASSET_SHOOT].animation.isPlaying)
        {
            spriteDrawTransparentAnimatedClipped(&_this.sprites[ASSET_SHOOT], _this.graphics.imageData, dt);
        }
        else if (glfwGetKey(_this.graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {

            if (_this.quadPosition != QPOS_NONE)
            {
                _this.sprites[ASSET_SHOOT].position = _this.sprites[ASSET_SIGHT].position;
                _this.sprites[ASSET_SHOOT].position.x -= _this.sprites[ASSET_SIGHT].size.x / 2;
                _this.sprites[ASSET_SHOOT].position.y -= _this.sprites[ASSET_SIGHT].size.y / 2;

                spriteDrawTransparentAnimatedClipped(&_this.sprites[ASSET_SHOOT], _this.graphics.imageData, dt);
                soundPlaySfx(_this.sound, SFX_SHOOT_HERO);
            }
        }

        printFPS(_this.graphics, dt);
        graphicsSwapBuffers(_this.graphics);
        glfwPollEvents();
    }

    return _this;
}

Program programCreate()
{
    staticAllocatorInit(100092024);
    Program this = {0};
    this.graphics = graphicsCreate(320, 240);
    this.sound = soundCreate();
    spritesLoad(this.sprites);
    Soloud_setGlobalVolume(this.sound.soloud, 1.);
    this.mainMenu = level1Create(this.graphics, this.sprites, this.sound);

    return this;
}

void programMainLoop(Program this)
{
    while (!this.mainMenu.shouldQuit)
    {
        this.mainMenu = level1Update(this.mainMenu);
        if (this.mainMenu.shouldQuit)
            return;
    }
}

void programDestroy(Program this)
{
    graphicsDestroy(this.graphics);
    staticAllocatorDestroy();
}

int main(void)
{

    Program this = programCreate();
    programMainLoop(this);
    programDestroy(this);

    return 0;
}