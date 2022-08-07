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

typedef enum
{
    ASSET_BACKGROUND,
    ASSET_SIGHT,
    ASSET_SHOOT,
    ASSET_ENEMY_GREEN_BIG,
    ASSET_ENEMY_GREEN_BIG_SHOOT,
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
    int spriteId;
    PointI basePosition;
    PointF position;
    EnemyState state;
} Enemy;

#define ENEMY_SPEED 50.
#define ENEMY_DOWN_OFFSET 30
#define ENEMY_UP_OFFSET 20

typedef enum
{
    QPOS_NONE,
    QPOS_BOTTOM,
    QPOS_BOTTOM_RIGHT,
    QPOS_BOTTOM_LEFT,
    QPOS_TOP,
    QPOS_TOP_RIGHT,
    QPOS_TOP_LEFT,
    QPOS_RIGHT,
    QPOS_LEFT,
    QPOS_COUNT
} QuadrantPosition;

typedef struct
{
    Graphics graphics;
    Sprite background;
    Sprite enemyGreenBig;
    Sprite enemyGreenSmall;
    Sprite sight;
    Sprite shoot;
    PointI textPosition;
    QuadrantPosition quadPosition;
    Sound sound;
    bool shouldQuit;
    Enemy enemyBigs[8];
    PointI positions[QPOS_COUNT];
} Level1;

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

    this[ASSET_ENEMY_GREEN_SMALL] = spriteCreate("assets/enemyGreenSmall2.bmp");
    this[ASSET_ENEMY_GREEN_SMALL_SHOOT] = spriteCreate("assets/enemyGreenSmall1.bmp");
}

PointI ENEMY_OFFSET = {-10, -10};

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

static int sortByStateHiddenFirst(Enemy *a, Enemy *b)
{
    return a->state == ENEMY_STATE_HIDDEN ? 1 : 0;
}

static int sortByStateGoingUpFirst(Enemy *a, Enemy *b)
{
    return a->state == ENEMY_STATE_GOING_UP ? 1 : 0;
}

void enemyProcessStateGoingDown(Enemy *enemies, float deltaTime)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_DOWN)
            continue;
        if (enemies[i].position.y > enemies[i].basePosition.y + ENEMY_DOWN_OFFSET)
        {
            enemies[i] = enemyPassToStateHidden(enemies[i]);
            continue;
        }
        enemies[i].position.y += ENEMY_SPEED * deltaTime;
    }
}

void enemyProcessStateIdle(Enemy *enemies)
{
    qsort(enemies, 8, sizeof(Enemy), sortByStateHiddenFirst);
}

void enemyProcessStateGoingUp(Enemy *enemies, float deltaTime)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_UP)
            continue;
        if (enemies[i].position.y < enemies[i].basePosition.y - ENEMY_UP_OFFSET)
        {
            enemies[i] = enemyPassToStateGoingDown(enemies[i]);
            continue;
        }
        enemies[i].position.y -= ENEMY_SPEED * deltaTime;
    }
}

static void setPositions(PointI *positions)
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

static Level1 initializeEnemies(Level1 _this)
{
    for (int i = 0; i < 8; i++)
    {
        _this.enemyBigs[i].position.x = _this.enemyBigs[i].basePosition.x = (int)_this.positions[i].x;
        _this.enemyBigs[i].position.y = _this.enemyBigs[i].basePosition.y = (int)_this.positions[i].y;
        _this.enemyBigs[i] = enemyPassToStateHidden(_this.enemyBigs[i]);
    }
    return _this;
}

Level1 handleControls(Level1 _this)
{

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_TOP_LEFT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP_LEFT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_TOP_RIGHT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_BOTTOM_LEFT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM_LEFT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_BOTTOM_RIGHT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_TOP];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_TOP;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_BOTTOM];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_BOTTOM;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_RIGHT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        _this.sight.position = _this.positions[QPOS_LEFT];
        spriteDrawTransparentClipped(_this.sight, _this.graphics.imageData);
        _this.quadPosition = QPOS_LEFT;
        return _this;
    }

    _this.quadPosition = QPOS_NONE;
    return _this;
}

Level1 mainMenuCreate(Graphics graphics, Sprite *sprites, Sound sound)
{
    Level1 _this = {0};

    setPositions(_this.positions);

    _this.sound = sound;
    _this.graphics = graphics;
    _this.background = sprites[ASSET_BACKGROUND];
    _this.enemyGreenBig = sprites[ASSET_ENEMY_GREEN_BIG];
    _this.enemyGreenSmall = sprites[ASSET_ENEMY_GREEN_SMALL];

    ENEMY_OFFSET.y = _this.graphics.imageData.size.y - _this.enemyGreenBig.size.y - _this.positions[QPOS_BOTTOM_LEFT].y;
    ENEMY_OFFSET.x = 40 - _this.positions[QPOS_BOTTOM_LEFT].x;

    _this.shoot = sprites[ASSET_SHOOT];
    _this.sight = sprites[ASSET_SIGHT];
    _this.shouldQuit = false;

    _this = initializeEnemies(_this);

    for (int i = 0; i < 8; i++)
    {
        _this.enemyBigs[i] = enemyPassToStateHidden(_this.enemyBigs[i]);
    }

    return _this;
}

static void enemiesDraw(Level1 _this)
{
    for (int i = 0; i < 8; i++)
    {
        if (_this.enemyBigs[i].state == ENEMY_STATE_HIDDEN)
            continue;
        _this.enemyGreenBig.position.x = (int)_this.enemyBigs[i].position.x;
        _this.enemyGreenBig.position.y = (int)_this.enemyBigs[i].position.y;
    };
    spriteDrawTransparentClipped(_this.enemyGreenBig, _this.graphics.imageData);
}

Level1 mainMenuUpdate(Level1 _this)
{
    soundPlaySpeech(_this.sound, SPEECH_SELECT_SHIP);
    bool shouldContinue = true;
    double elapsedTime = glfwGetTime();
    while (shouldContinue && !_this.shouldQuit)
    {
        float dt = getDeltaTime();
        graphicsClear(_this.graphics.imageData);

        _this.shouldQuit = isKeyJustPressed(_this.graphics.window, GLFW_KEY_ESCAPE);
        spriteDrawClipped(_this.background, _this.graphics.imageData);

        if (glfwGetTime() - elapsedTime > 1.)
        {
            elapsedTime = glfwGetTime();
            int enemyToDisplay = 1 + rand() % 3;
            if (_this.enemyBigs[enemyToDisplay].state == ENEMY_STATE_HIDDEN)
                _this.enemyBigs[enemyToDisplay] = enemyPassToStateGoingUp(_this.enemyBigs[enemyToDisplay]);
        }

        enemyProcessStateGoingDown(_this.enemyBigs, dt);
        enemyProcessStateGoingUp(_this.enemyBigs, dt);

        enemiesDraw(_this);

        _this = handleControls(_this);

        if (_this.shoot.animation.isPlaying)
        {
            spriteDrawTransparentAnimatedClipped(&_this.shoot, _this.graphics.imageData, dt);
        }
        else if (glfwGetKey(_this.graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {

            if (_this.quadPosition != QPOS_NONE)
            {
                _this.shoot.position = _this.sight.position;
                _this.shoot.position.x -= _this.sight.size.x / 2;
                _this.shoot.position.y -= _this.sight.size.y / 2;
                spriteDrawTransparentAnimatedClipped(&_this.shoot, _this.graphics.imageData, dt);
                soundPlaySfx(_this.sound, SFX_SHOOT_HERO);
            }
        }

        printFPS(_this.graphics, dt);
        graphicsSwapBuffers(_this.graphics);
        glfwPollEvents();
    }

    return _this;
}

typedef struct
{
    Graphics graphics;
    Level1 mainMenu;
    Sprite sprites[ASSET_COUNT];
    Sound sound;
} Program;

#define STBI_MALLOC(sz) allocStatic(sz)
#define STBI_REALLOC(p, newsz) reallocStatic(p, newsz)
#define STBI_FREE(p) freeStatic(p)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Program programCreate()
{
    staticAllocatorInit(100092024);
    Program this = {0};
    this.graphics = graphicsCreate(320, 240);
    this.sound = soundCreate();
    spritesLoad(this.sprites);
    Soloud_setGlobalVolume(this.sound.soloud, 1.);
    this.mainMenu = mainMenuCreate(this.graphics, this.sprites, this.sound);

    return this;
}

void programMainLoop(Program this)
{
    while (!this.mainMenu.shouldQuit)
    {
        this.mainMenu = mainMenuUpdate(this.mainMenu);
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