#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <soloud_c.h>
#include "program/core/utils/utils.h"
#include "program/core/graphics/graphics.h"
#include "program/core/sprite/sprite.h"
#include "program/core/input/keyboard.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

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

#define ENEMY_DOWN_OFFSET 53
#define ENEMY_UP_OFFSET 20

#include <soloud_c.h>

typedef enum GaneSfx
{
    SFX_SELECT,
    SFX_SHOOT_HERO,
    SFX_ENEMY_ESCAPED,
    SFX_BLIP,
    SFX_HERO_JUMP,
    SFX_HERO_HURT,
    SFX_COUNT
} GaneSfx;

typedef enum GameSpeech
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

typedef struct
{
    Graphics graphics;
    Sprite sprites[ASSET_COUNT];
    Sound sound;
    bool shouldQuit;
    bool shouldStop;
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
    printFPS(graphics, deltaTime);
    graphicsSwapBuffers(graphics);
    glfwPollEvents();
}

typedef struct
{
    PointI textPosition;
    QuadrantPosition quadPosition;
    Sound sound;
    float enemySpeed;
    Enemy enemies[8];
    PointI positions[QPOS_COUNT];
    int enemiesRemaining;
    int enemiesKilled;
    GameState gameState;
    char *enemiesRemainingString[100];
    char *enemiesKilledString[100];
    char *percentageKilledString[100];
    double elapsedTime;
    double gameElapsedTime;
    PointI enemyOffset;
} Level1;

typedef struct
{
    GameState gameState;
} Program;

Sound soundCreate()
{
    Sound this = {0};

    this.soloud = Soloud_create();
    for (int i = 0; i < SPEECH_COUNT; i++)
    {
        this.speechs[i] = Speech_create();
    }

    for (int i = 0; i < SFX_COUNT; i++)
    {
        this.sfx[i] = Sfxr_create();
    }

    Speech_setText(this.speechs[SPEECH_JUMP_THE_ROCKS], "Jump the rocks!");
    Speech_setText(this.speechs[SPEECH_SHOOT_THE_BAD_GUYS], "Shoot the bad guys!");
    Speech_setText(this.speechs[SPEECH_NOOO], "O!");
    Sfxr_loadPreset(this.sfx[SFX_BLIP], SFXR_BLIP, 3247);
    Sfxr_loadPreset(this.sfx[SFX_SELECT], SFXR_POWERUP, 3247);
    Sfxr_loadPreset(this.sfx[SFX_SHOOT_HERO], SFXR_EXPLOSION, 3247);
    Sfxr_loadPreset(this.sfx[SFX_ENEMY_ESCAPED], SFXR_HURT, 3247);
    Sfxr_loadPreset(this.sfx[SFX_HERO_JUMP], SFXR_JUMP, 3247);
    Sfxr_loadPreset(this.sfx[SFX_HERO_HURT], SFXR_HURT, 3247);

    Soloud_initEx(this.soloud, SOLOUD_CLIP_ROUNDOFF | SOLOUD_ENABLE_VISUALIZATION,
                  SOLOUD_AUTO, SOLOUD_AUTO, SOLOUD_AUTO, 2);

    Soloud_setGlobalVolume(this.soloud, 4);

    return this;
}

void soundPlaySfx(Sound this, GaneSfx id)
{
    Soloud_play(this.soloud, this.sfx[id]);
}

void soundPlaySpeech(Sound this, GameSpeech id)
{
    Soloud_play(this.soloud, this.speechs[id]);
}

void soundDestroy(Sound this)
{
    for (int i = 0; i < SPEECH_COUNT; i++)
    {
        Speech_destroy(this.speechs[i]);
    }

    for (int i = 0; i < SFX_COUNT; i++)
    {
        Sfxr_destroy(this.sfx[i]);
    }

    Soloud_deinit(this.soloud);
    Soloud_destroy(this.soloud);
}

void loadAssets(Sprite *_this)
{
    _this[ASSET_BACKGROUND] = spriteCreate("assets/background.bmp");
    _this[ASSET_FOREGROUND] = spriteCreate("assets/foreground.bmp");
    _this[ASSET_TOP_SCORE_SQUARE] = spriteCreate("assets/topScoreSquare.bmp");
    _this[ASSET_TOP_SCORE_SQUARE].position.x = 32;

    _this[ASSET_HOW_TO_PLAY] = spriteCreate("assets/howToPlay.bmp");
    _this[ASSET_HOW_TO_PLAY].position.x = 119;
    _this[ASSET_HOW_TO_PLAY].position.y = 130;
    _this[ASSET_HOW_TO_PLAY].animated = true;
    _this[ASSET_HOW_TO_PLAY].animation.frameCount = 5;
    _this[ASSET_HOW_TO_PLAY].animation.frameRate = 5;
    _this[ASSET_HOW_TO_PLAY].animation.frameWidth = 82;

    _this[ASSET_SIGHT] = spriteCreate("assets/aimcross.png");

    _this[ASSET_SHOOT] = spriteCreate("assets/shoot.bmp");
    _this[ASSET_SHOOT].animated = true;
    _this[ASSET_SHOOT].animation.frameCount = 4;
    _this[ASSET_SHOOT].animation.currentFrame = 0;
    _this[ASSET_SHOOT].animation.frameWidth = 55;
    _this[ASSET_SHOOT].animation.frameRate = 10.;

    _this[ASSET_ENEMY_GREEN_BIG] = spriteCreate("assets/enemyGreenBig2.bmp");
    _this[ASSET_ENEMY_GREEN_BIG_SHOOT] = spriteCreate("assets/enemyGreenBig1.bmp");

    _this[ASSET_ENEMY_GREEN_MEDIUM] = spriteCreate("assets/enemyGreenSmall2.bmp");
    _this[ASSET_ENEMY_GREEN_MEDIUM_SHOOT] = spriteCreate("assets/enemyGreenSmall1.bmp");

    _this[ASSET_ENEMY_GREEN_SMALL] = spriteCreate("assets/enemyGreenTiny2.bmp");
    _this[ASSET_ENEMY_GREEN_SMALL_SHOOT] = spriteCreate("assets/enemyGreenTiny1.bmp");

    _this[ASSET_LEVEL2_BACKGROUND] = spriteCreate("assets/level2/background.bmp");
    _this[ASSET_LEVEL2_CLOUD_1] = spriteCreate("assets/level2/cloud3.bmp");
    _this[ASSET_LEVEL2_CLOUD_2] = spriteCreate("assets/level2/cloud2.bmp");
    _this[ASSET_LEVEL2_CLOUD_3] = spriteCreate("assets/level2/cloud1.bmp");
    _this[ASSET_LEVEL2_CLOUD_4] = spriteCreate("assets/level2/cloud0.bmp");

    _this[ASSET_LEVEL2_HERO_GREEN] = spriteCreate("assets/level2/heroGreen.bmp");
    _this[ASSET_LEVEL2_HERO_RED] = spriteCreate("assets/level2/heroRed.bmp");
    _this[ASSET_LEVEL2_HERO_BLUE] = spriteCreate("assets/level2/heroBlue.bmp");
    _this[ASSET_LEVEL2_HERO_YELLOW] = spriteCreate("assets/level2/heroYellow.bmp");

    _this[ASSET_LEVEL2_OBSTACLE_1] = spriteCreate("assets/level2/tree1.bmp");
    _this[ASSET_LEVEL2_OBSTACLE_2] = spriteCreate("assets/level2/tree2.bmp");
    _this[ASSET_LEVEL2_OBSTACLE_3] = spriteCreate("assets/level2/tree3.bmp");

    _this[ASSET_LEVEL2_HOW_TO_PLAY] = spriteCreate("assets/level2/howToPlay.bmp");
    _this[ASSET_LEVEL2_HOW_TO_PLAY].position.x = 119;
    _this[ASSET_LEVEL2_HOW_TO_PLAY].position.y = 130;
    _this[ASSET_LEVEL2_HOW_TO_PLAY].animated = true;
    _this[ASSET_LEVEL2_HOW_TO_PLAY].animation.frameCount = 2;
    _this[ASSET_LEVEL2_HOW_TO_PLAY].animation.frameRate = 10;
    _this[ASSET_LEVEL2_HOW_TO_PLAY].animation.frameWidth = 82;

    _this[ASSET_LEVEL2_HERO_GREEN].position.x = 80;
    _this[ASSET_LEVEL2_HERO_GREEN].position.y = 174;
    _this[ASSET_LEVEL2_HERO_GREEN].animated = true;
    _this[ASSET_LEVEL2_HERO_GREEN].animation.frameCount = 6;
    _this[ASSET_LEVEL2_HERO_GREEN].animation.frameWidth = 24;
    _this[ASSET_LEVEL2_HERO_GREEN].animation.frameRate = 15;

    _this[ASSET_LEVEL2_HERO_BLUE].position.x = 70;
    _this[ASSET_LEVEL2_HERO_BLUE].position.y = 174;
    _this[ASSET_LEVEL2_HERO_BLUE].animated = true;
    _this[ASSET_LEVEL2_HERO_BLUE].animation.frameCount = 6;
    _this[ASSET_LEVEL2_HERO_BLUE].animation.frameWidth = 24;
    _this[ASSET_LEVEL2_HERO_BLUE].animation.frameRate = 15;

    _this[ASSET_LEVEL2_HERO_RED].position.x = 60;
    _this[ASSET_LEVEL2_HERO_RED].position.y = 174;
    _this[ASSET_LEVEL2_HERO_RED].animated = true;
    _this[ASSET_LEVEL2_HERO_RED].animation.frameCount = 6;
    _this[ASSET_LEVEL2_HERO_RED].animation.frameWidth = 24;
    _this[ASSET_LEVEL2_HERO_RED].animation.frameRate = 15;

    _this[ASSET_LEVEL2_HERO_YELLOW].position.x = 50;
    _this[ASSET_LEVEL2_HERO_YELLOW].position.y = 174;
    _this[ASSET_LEVEL2_HERO_YELLOW].animated = true;
    _this[ASSET_LEVEL2_HERO_YELLOW].animation.frameCount = 6;
    _this[ASSET_LEVEL2_HERO_YELLOW].animation.frameWidth = 24;
    _this[ASSET_LEVEL2_HERO_YELLOW].animation.frameRate = 15;
    _this[ASSET_LEVEL2_OBSTACLE_1].position.y = 179;

    _this[ASSET_LEVEL3_BACKGROUND] = spriteCreate("assets/level3/background.bmp");
    _this[ASSET_LEVEL3_BACKGROUND_TILED] = spriteCreate("assets/level3/background-tiled.bmp");

    _this[ASSET_LEVEL3_HERO_JUMP] = spriteCreate("assets/level3/playerJump.bmp");
    _this[ASSET_LEVEL3_HERO_JUMP].center.x = -16;
    _this[ASSET_LEVEL3_HERO_JUMP].center.y = -32;

    _this[ASSET_LEVEL3_HERO_FALL] = spriteCreate("assets/level3/playerFall.bmp");
    _this[ASSET_LEVEL3_HERO_FALL].center.x = -16;
    _this[ASSET_LEVEL3_HERO_FALL].center.y = -32;

    _this[ASSET_LEVEL3_HERO_RUN] = spriteCreate("assets/level3/playerRun.bmp");
    _this[ASSET_LEVEL3_HERO_RUN].position.x = 0;
    _this[ASSET_LEVEL3_HERO_RUN].position.y = 0;
    _this[ASSET_LEVEL3_HERO_RUN].animated = true;
    _this[ASSET_LEVEL3_HERO_RUN].animation.frameCount = 12;
    _this[ASSET_LEVEL3_HERO_RUN].animation.frameWidth = 32;
    _this[ASSET_LEVEL3_HERO_RUN].animation.frameRate = 15;
    _this[ASSET_LEVEL3_HERO_RUN].center.x = -16;
    _this[ASSET_LEVEL3_HERO_RUN].center.y = -32;

    _this[ASSET_LEVEL3_HERO_IDLE] = spriteCreate("assets/level3/playerIdle.bmp");
    _this[ASSET_LEVEL3_HERO_IDLE].position.x = 0;
    _this[ASSET_LEVEL3_HERO_IDLE].position.y = 0;
    _this[ASSET_LEVEL3_HERO_IDLE].animated = true;
    _this[ASSET_LEVEL3_HERO_IDLE].animation.frameCount = 12;
    _this[ASSET_LEVEL3_HERO_IDLE].animation.frameWidth = 32;
    _this[ASSET_LEVEL3_HERO_IDLE].animation.frameRate = 15;
    _this[ASSET_LEVEL3_HERO_IDLE].center.x = -16;
    _this[ASSET_LEVEL3_HERO_IDLE].center.y = -32;
}

Enemy enemyPassToStateHidden(Enemy _this)
{
    _this.visible = true;
    _this.state = ENEMY_STATE_HIDDEN;
    _this.position.y = _this.basePosition.y + ENEMY_DOWN_OFFSET;
    if (_this.spriteId > ASSET_ENEMY_GREEN_SMALL)
        _this.spriteId -= 3;
    return _this;
}

Enemy enemyPassToStateDead(Enemy _this)
{
    if (_this.state == ENEMY_STATE_DEAD || _this.state == ENEMY_STATE_HIDDEN)
        return _this;
    _this.state = ENEMY_STATE_DEAD;
    _this.spriteId += 3;
    _this.elapsedStateTime = 0;

    return _this;
}

Enemy enemyPassToStateGoingUp(Enemy _this)
{
    _this.state = ENEMY_STATE_GOING_UP;
    _this.position.y = _this.basePosition.y + ENEMY_DOWN_OFFSET;
    return _this;
}

Enemy enemyPassToStateGoingDown(Enemy _this)
{
    _this.state = ENEMY_STATE_GOING_DOWN;
    return _this;
}

void enemyProcessStateGoingDownTutorial(Enemy *enemies, float deltaTime, double enemySpeed)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_DOWN)
            continue;
        if (enemies[i].position.y > enemies[i].lowerClippingPosition)
        {
            enemies[i] = enemyPassToStateHidden(enemies[i]);
            continue;
        }

        enemies[i].position.y += enemies[i].speedMultiplicator * enemySpeed * deltaTime;
    }
}

void enemyProcessStateGoingDown(Enemy *enemies, float deltaTime, double enemySpeed, int *enemiesRemaining, Sound sound)
{
    float speedMultiplier = 1.;
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_DOWN)
            continue;
        if (enemies[i].position.y > enemies[i].lowerClippingPosition)
        {
            enemies[i] = enemyPassToStateHidden(enemies[i]);
            soundPlaySfx(sound, SFX_ENEMY_ESCAPED);
            if (enemiesRemaining != NULL)
                *enemiesRemaining -= 1;
            continue;
        }

        enemies[i].position.y += enemies[i].speedMultiplicator * enemySpeed * deltaTime;
    }
}

void enemyProcessStateGoingUp(Enemy *enemies, float deltaTime, double enemySpeed)
{
    float speedMultiplier = 1.;
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_UP)
            continue;
        if (enemies[i].position.y < enemies[i].topLimit)
        {
            enemies[i] = enemyPassToStateGoingDown(enemies[i]);
            continue;
        }

        enemies[i].position.y -= enemies[i].speedMultiplicator * enemySpeed * deltaTime;
    }
}

void enemyProcessStateDead(Enemy *enemies, float deltaTime)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_DEAD)
            continue;

        enemies[i].elapsedStateTime += deltaTime;

        if (enemies[i].elapsedStateTime > 1)
        {
            enemies[i] = enemyPassToStateHidden(enemies[i]);
            continue;
        }

        int reminder = (int)floorf(enemies[i].elapsedStateTime * 1000) % 2;

        if (reminder == 0)
            enemies[i].visible = false;
        else
            enemies[i].visible = true;
    }
}

void level1InitPositions(PointI *positions)
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
    positions[QPOS_BOTTOM_RIGHT].y = 200;

    positions[QPOS_BOTTOM].x = 125;
    positions[QPOS_BOTTOM].y = 200;

    positions[QPOS_BOTTOM_LEFT].x = 60;
    positions[QPOS_BOTTOM_LEFT].y = 200;

    positions[QPOS_LEFT].x = 55;
    positions[QPOS_LEFT].y = 125;

    positions[QPOS_TOP_LEFT].x = 42;
    positions[QPOS_TOP_LEFT].y = 55;
}

static Level1 level1InitEnemies(Level1 _this)
{
    _this.enemies[QPOS_TOP_LEFT].lowerClippingPosition = 92;
    _this.enemies[QPOS_TOP].lowerClippingPosition = 94;
    _this.enemies[QPOS_TOP_RIGHT].lowerClippingPosition = 102;
    _this.enemies[QPOS_LEFT].lowerClippingPosition = 157;
    _this.enemies[QPOS_RIGHT].lowerClippingPosition = 177;
    _this.enemies[QPOS_BOTTOM_LEFT].lowerClippingPosition = 240;
    _this.enemies[QPOS_BOTTOM_RIGHT].lowerClippingPosition = 240;
    _this.enemies[QPOS_BOTTOM].lowerClippingPosition = 240;

    for (int i = 0; i < 8; i++)
    {
        _this.enemies[i].visible = true;
        _this.enemies[i].position.x = _this.enemies[i].basePosition.x = (int)_this.positions[i].x + _this.enemyOffset.x;
        _this.enemies[i].position.y = _this.enemies[i].basePosition.y = (int)_this.positions[i].y + _this.enemyOffset.y;
        _this.enemies[i] = enemyPassToStateHidden(_this.enemies[i]);

        if (i <= QPOS_TOP_RIGHT)
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_SMALL;
        else if (i >= QPOS_LEFT && i <= QPOS_RIGHT)
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_MEDIUM;
        else
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_BIG;

        _this.enemies[i].bottomOffset = _this.enemies[i].lowerClippingPosition;
        _this.enemies[i].topLimit = _this.enemies[i].lowerClippingPosition - _this.gameState.sprites[_this.enemies[i].spriteId].size.y;
        _this.enemies[i].speedMultiplicator = (float)_this.gameState.sprites[_this.enemies[i].spriteId].size.y / (float)_this.gameState.sprites[ASSET_ENEMY_GREEN_BIG].size.y;
    }
    for (int i = 0; i < 8; i++)
    {
        _this.enemies[i] = enemyPassToStateHidden(_this.enemies[i]);
    }

    for (int i = 0; i < 8; i++)
    {
        _this.enemies[i] = enemyPassToStateGoingUp(_this.enemies[i]);
    }
    return _this;
}

Level1 level1Tutorial(Level1 _this)
{
    while (!_this.gameState.shouldStop && !_this.gameState.shouldQuit)
    {
        int hiddenEnemies = 0;
        float dt = getDeltaTime();
        _this.gameState = gameStateCheckExitConditions(_this.gameState);

        enemyProcessStateGoingDownTutorial(_this.enemies, dt, _this.enemySpeed);
        enemyProcessStateGoingUp(_this.enemies, dt, _this.enemySpeed);

        for (int i = 0; i < 8; i++)
        {
            if (_this.enemies[i].state == ENEMY_STATE_HIDDEN)
                hiddenEnemies++;
        }

        if (hiddenEnemies == 8)
            _this.gameState.shouldStop = true;

        // DRAW
        // Draw Background
        spriteDrawClipped(_this.gameState.sprites[ASSET_BACKGROUND], _this.gameState.graphics.imageData);
        spriteDrawClipped(_this.gameState.sprites[ASSET_TOP_SCORE_SQUARE], _this.gameState.graphics.imageData);
        // Draw Enemies
        for (int i = 0; i < 8; i++)
        {
            _this.gameState.sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
            _this.gameState.sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;
            spriteDrawTransparentClippedLowerLine(_this.gameState.sprites[_this.enemies[i].spriteId], _this.gameState.graphics.imageData, _this.enemies[i].lowerClippingPosition);
        };

        spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_FOREGROUND], _this.gameState.graphics.imageData);
        _this.gameState.sprites[ASSET_HOW_TO_PLAY] = spriteDrawTransparentAnimatedClipped(_this.gameState.sprites[ASSET_HOW_TO_PLAY], _this.gameState.graphics.imageData, dt);
        swapBuffersPrintFPSPollEvents(_this.gameState.graphics, dt);
    }
    return _this;
}

Level1 level1GameLoop(Level1 _this)
{
    _this.gameState.shouldStop = false;

    while (!_this.gameState.shouldStop && !_this.gameState.shouldQuit && _this.enemiesRemaining > 0)
    {
        float dt = getDeltaTime();

        _this.gameState = gameStateCheckExitConditions(_this.gameState);

        // Enemy selection and trigger to attack
        if (glfwGetTime() - _this.elapsedTime > .5)
        {
            _this.elapsedTime = glfwGetTime();
            int enemyToDisplay = rand() % 8;

            if (_this.enemies[enemyToDisplay].state == ENEMY_STATE_HIDDEN)
                _this.enemies[enemyToDisplay] = enemyPassToStateGoingUp(_this.enemies[enemyToDisplay]);
        }

        // Rise the difficulty by ramping the speed of the enemies
        _this.gameElapsedTime += dt;

        _this.enemySpeed = fminf(100 + _this.gameElapsedTime, 300);

        enemyProcessStateGoingDown(_this.enemies, dt, _this.enemySpeed, &_this.enemiesRemaining, _this.gameState.sound);
        enemyProcessStateGoingUp(_this.enemies, dt, _this.enemySpeed);
        enemyProcessStateDead(_this.enemies, dt);
        // Handle Controls
        if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
            glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            _this.gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP_LEFT];
            _this.quadPosition = QPOS_TOP_LEFT;
            goto continueControls;
        }

        if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
            glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            _this.gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP_RIGHT];
            _this.quadPosition = QPOS_TOP_RIGHT;
            goto continueControls;
        }

        if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
            glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            _this.gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM_LEFT];
            _this.quadPosition = QPOS_BOTTOM_LEFT;
            goto continueControls;
        }

        if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
            glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            _this.gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM_RIGHT];
            _this.quadPosition = QPOS_BOTTOM_RIGHT;
            goto continueControls;
        }

        if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            _this.gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP];
            _this.quadPosition = QPOS_TOP;
            goto continueControls;
        }

        if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            _this.gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM];
            _this.quadPosition = QPOS_BOTTOM;
            goto continueControls;
        }

        if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            _this.gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_RIGHT];
            _this.quadPosition = QPOS_RIGHT;
            goto continueControls;
        }

        if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            _this.gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_LEFT];
            _this.quadPosition = QPOS_LEFT;
            goto continueControls;
        }
        else
            _this.quadPosition = QPOS_NONE;
    continueControls:
        // DRAW
        // Draw Background
        spriteDrawClipped(_this.gameState.sprites[ASSET_BACKGROUND], _this.gameState.graphics.imageData);
        spriteDrawClipped(_this.gameState.sprites[ASSET_TOP_SCORE_SQUARE], _this.gameState.graphics.imageData);

        // Draw Sight
        spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_SIGHT], _this.gameState.graphics.imageData);

        // Draw Enemies
        for (int i = 0; i < 8; i++)
        {
            if (_this.enemies[i].state == ENEMY_STATE_HIDDEN || !_this.enemies[i].visible)
                continue;

            _this.gameState.sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
            _this.gameState.sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;

            spriteDrawTransparentClippedLowerLine(_this.gameState.sprites[_this.enemies[i].spriteId], _this.gameState.graphics.imageData, _this.enemies[i].lowerClippingPosition);
        };

        if (_this.gameState.sprites[ASSET_SHOOT].animation.isPlaying)
        {
            _this.gameState.sprites[ASSET_SHOOT] = spriteDrawTransparentAnimatedClipped(_this.gameState.sprites[ASSET_SHOOT], _this.gameState.graphics.imageData, dt);
        }
        else if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {

            if (_this.quadPosition != QPOS_NONE)
            {
                _this.gameState.sprites[ASSET_SHOOT].position = _this.gameState.sprites[ASSET_SIGHT].position;
                _this.gameState.sprites[ASSET_SHOOT].position.x -= _this.gameState.sprites[ASSET_SIGHT].size.x / 2;
                _this.gameState.sprites[ASSET_SHOOT].position.y -= _this.gameState.sprites[ASSET_SIGHT].size.y / 2;

                _this.gameState.sprites[ASSET_SHOOT] = spriteDrawTransparentAnimatedClipped(_this.gameState.sprites[ASSET_SHOOT], _this.gameState.graphics.imageData, dt);

                if (_this.enemies[_this.quadPosition].state != ENEMY_STATE_HIDDEN && _this.enemies[_this.quadPosition].state != ENEMY_STATE_DEAD)
                {
                    soundPlaySpeech(_this.gameState.sound, SPEECH_NOOO);
                    _this.enemiesRemaining--;
                    _this.enemiesKilled++;
                    _this.enemies[_this.quadPosition] = enemyPassToStateDead(_this.enemies[_this.quadPosition]);
                }
                soundPlaySfx(_this.gameState.sound, SFX_SHOOT_HERO);
            }
        }

        spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_FOREGROUND], _this.gameState.graphics.imageData);
        snprintf(_this.enemiesRemainingString, 100, "enemies remaining: %d", _this.enemiesRemaining);
        snprintf(_this.enemiesKilledString, 100, "enemies killed: %d", _this.enemiesKilled);
        float enemiesKilledPercentage = _this.enemiesKilled / (100. - _this.enemiesRemaining) * 100.;
        snprintf(_this.percentageKilledString, 100, "hit percentage: %.0f%%", enemiesKilledPercentage);
        graphicsPrintString(_this.gameState.graphics.imageData, (PointI){100, 10}, _this.enemiesRemainingString, (Color){0xFF, 0xFF, 0xFF});
        graphicsPrintString(_this.gameState.graphics.imageData, (PointI){100, 20}, _this.enemiesKilledString, (Color){0xFF, 0xFF, 0xFF});
        graphicsPrintString(_this.gameState.graphics.imageData, (PointI){100, 30}, _this.percentageKilledString, (Color){0xFF, 0xFF, 0xFF});

        swapBuffersPrintFPSPollEvents(_this.gameState.graphics, dt);
    }
    return _this;
}

Level1 level1Create()
{
    Level1 _this = {0};
    _this.enemyOffset.y = 0;
    _this.enemyOffset.x = -8;
    _this.elapsedTime = glfwGetTime();
    _this.enemySpeed = 100.;
    _this.enemiesRemaining = 99;
    return _this;
}

Level1 level1GameCompleteLoop(Level1 _this)
{
    float statisticsOffset = 0.;
    _this.gameState.shouldStop = false;
    while (!_this.gameState.shouldStop && !_this.gameState.shouldQuit)
    {
        float statisticsSpeed = 100.;
        float dt = getDeltaTime();
        _this.gameState = gameStateCheckExitConditions(_this.gameState);

        spriteDrawClipped(_this.gameState.sprites[ASSET_BACKGROUND], _this.gameState.graphics.imageData);
        // Draw Enemies
        for (int i = 0; i < 8; i++)
        {
            _this.gameState.sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
            _this.gameState.sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;

            spriteDrawTransparentClippedLowerLine(_this.gameState.sprites[_this.enemies[i].spriteId], _this.gameState.graphics.imageData, _this.enemies[i].lowerClippingPosition);
        };

        spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_FOREGROUND], _this.gameState.graphics.imageData);

        statisticsOffset = fminf(statisticsOffset + statisticsSpeed * dt, 100.);

        if (statisticsOffset == 100.)
        {
            _this.gameState.shouldStop = true;
        }

        _this.gameState.sprites[ASSET_TOP_SCORE_SQUARE].position.y = statisticsOffset;
        spriteDrawClipped(_this.gameState.sprites[ASSET_TOP_SCORE_SQUARE], _this.gameState.graphics.imageData);
        graphicsPrintString(_this.gameState.graphics.imageData, (PointI){100, 20 + statisticsOffset}, _this.enemiesKilledString, (Color){0xFF, 0xFF, 0xFF});
        graphicsPrintString(_this.gameState.graphics.imageData, (PointI){100, 30 + statisticsOffset}, _this.percentageKilledString, (Color){0xFF, 0xFF, 0xFF});

        swapBuffersPrintFPSPollEvents(_this.gameState.graphics, dt);
    }

    // Game Score Delay
    _this.gameState.shouldStop = false;
    float elapsedTimeSinceScoreDisplay = 0;
    while (!_this.gameState.shouldStop && !_this.gameState.shouldQuit && elapsedTimeSinceScoreDisplay < 2.)
    {
        float deltaTime = getDeltaTime();
        _this.gameState = gameStateCheckExitConditions(_this.gameState);

        elapsedTimeSinceScoreDisplay += deltaTime;
        glfwPollEvents();
    }
    _this.gameState.shouldStop = true;
    return _this;
}

typedef struct
{
    float backgroundSpeed;
    float backgroundAcceleration;
    bool commands[4];
    float gravity;
    float subpixelPosition[4];
    float screenPosition;
    float baseVerticalSpeed;
    float baseBackgroundSpeed;
    float verticalSpeed[4];
    float elapsedTimeSinceJump;
    float elapsedTimeSinceHit;
    float clowdSpeedRatio[4];
    float cloudPosition[4];
    float obstaclePosition;
    bool collided;
    int livesLost;
    float elapsedTimeBlink;
    GameState gameState;
    float runningDistance;
} Level2;

Level2 level2Create()
{
    Level2 _this = {0};

    _this.baseVerticalSpeed = -300.;
    _this.baseBackgroundSpeed = -250.;

    _this.backgroundSpeed = _this.baseBackgroundSpeed;
    _this.backgroundAcceleration = -5.0;
    _this.gravity = 1000.;
    _this.elapsedTimeSinceHit = 10000;

    _this.subpixelPosition[0] = 174.;
    _this.subpixelPosition[1] = 174.;
    _this.subpixelPosition[2] = 174.;
    _this.subpixelPosition[3] = 174.;

    _this.clowdSpeedRatio[0] = .05;
    _this.clowdSpeedRatio[1] = .1;
    _this.clowdSpeedRatio[2] = .2;
    _this.clowdSpeedRatio[3] = .22;

    _this.obstaclePosition = 320.;
    _this.collided = false;

    return _this;
}

Level2 level2TutorialLoop(Level2 _this)
{
    while (!_this.gameState.shouldStop && !_this.gameState.shouldQuit)
    {
        float deltaTime = getDeltaTime();
        _this.gameState = gameStateCheckExitConditions(_this.gameState);

        float backgroundSpeedFrame = deltaTime * _this.backgroundSpeed;

        // Clowds movement
        for (int i = 0; i < 4; i++)
        {
            _this.cloudPosition[i] += backgroundSpeedFrame * _this.clowdSpeedRatio[i];
            if (_this.cloudPosition[i] < -_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x)
            {
                _this.cloudPosition[i] = 0.;
            }
        }

        if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_SPACE) && _this.subpixelPosition[_this.livesLost] == 174.)
        {
            _this.gameState.shouldStop = true;

            _this.elapsedTimeSinceJump = 0;
            for (int i = _this.livesLost; i < 4; i++)
            {
                _this.commands[i] = true;
            }
        }
        // =================================================================
        // RENDER SECTION
        // =================================================================
        // Draw background clouds
        for (int i = 0; i < 4; i++)
        {
            _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i];
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i], _this.gameState.graphics.imageData);
            _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i] + _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i], _this.gameState.graphics.imageData);
        }

        // Draw Background
        {
            _this.screenPosition += backgroundSpeedFrame;
            _this.screenPosition = _this.screenPosition > -320. ? _this.screenPosition : 0.;
            _this.gameState.sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_BACKGROUND], _this.gameState.graphics.imageData);
            _this.gameState.sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition + 320;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_BACKGROUND], _this.gameState.graphics.imageData);
        }

        // Draw Heroes
        {
            _this.elapsedTimeSinceHit += deltaTime;
            int elapsedTimeSinceHitI = _this.elapsedTimeSinceHit;
            elapsedTimeSinceHitI = (_this.elapsedTimeSinceHit - elapsedTimeSinceHitI) * 100;
            float distanceBetweenDynos = _this.backgroundSpeed / _this.baseBackgroundSpeed;
            _this.gameState.sprites[ASSET_LEVEL2_HERO_BLUE].position.x = _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 10. * distanceBetweenDynos;
            _this.gameState.sprites[ASSET_LEVEL2_HERO_RED].position.x = _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 20. * distanceBetweenDynos;
            _this.gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].position.x = _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 30. * distanceBetweenDynos;

            for (int i = 3; i >= 0; i--)
            {
                _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + i].animation.frameRate = 15. * distanceBetweenDynos;
                _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + i] = spriteDrawTransparentAnimatedClipped(_this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + i], _this.gameState.graphics.imageData, deltaTime);
            }
        }
        // Draw UI
        _this.gameState.sprites[ASSET_LEVEL2_HOW_TO_PLAY] = spriteDrawTransparentAnimatedClipped(_this.gameState.sprites[ASSET_LEVEL2_HOW_TO_PLAY], _this.gameState.graphics.imageData, deltaTime);
        swapBuffersPrintFPSPollEvents(_this.gameState.graphics, deltaTime);
    }
    _this.gameState.shouldStop = false;
    return _this;
}

Level2 level2GameLoop(Level2 _this)
{
    while (!_this.gameState.shouldStop && !_this.gameState.shouldQuit && _this.runningDistance < 316)
    {
        float deltaTime = getDeltaTime();
        _this.gameState = gameStateCheckExitConditions(_this.gameState);

        _this.backgroundSpeed += _this.backgroundAcceleration * deltaTime;
        float backgroundSpeedFrame = deltaTime * _this.backgroundSpeed;

        // Clowds movement
        for (int i = 0; i < 4; i++)
        {
            _this.cloudPosition[i] += backgroundSpeedFrame * _this.clowdSpeedRatio[i];
            if (_this.cloudPosition[i] < -_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x)
            {
                _this.cloudPosition[i] = 0.;
            }
        }

        static char obstacleStream[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1};
        static int matrixSize = sizeof(obstacleStream) / sizeof(obstacleStream[0]);

        // Obstacles & collision
        {
            _this.collided = false;
            _this.obstaclePosition += backgroundSpeedFrame;

            if (_this.obstaclePosition < -_this.gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * matrixSize)
                _this.obstaclePosition = 320. + _this.gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x;

            PointI distance;
            for (int i = 0; i < matrixSize; i++)
            {
                if (obstacleStream[i] == 1)
                {
                    static PointI obstaclePosV = {0, 174.};
                    obstaclePosV.x = _this.obstaclePosition + _this.gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * i;
                    distance.x = obstaclePosV.x +
                                 _this.gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x / 2 -
                                 _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + _this.livesLost].position.x -
                                 _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + _this.livesLost].animation.frameWidth;
                    distance.y = obstaclePosV.y - _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + _this.livesLost].position.y;
                    float distanceScalar = distance.x * distance.x + distance.y * distance.y;
                    if (50 > distanceScalar)
                    {
                        _this.collided = true;
                    }
                }
            }

            if (_this.collided && _this.elapsedTimeSinceHit > 1.)
            {
                _this.elapsedTimeSinceHit = 0;
                soundPlaySfx(_this.gameState.sound, SFX_HERO_HURT);
                _this.backgroundSpeed = _this.baseBackgroundSpeed;
                _this.verticalSpeed[_this.livesLost] = _this.baseVerticalSpeed;
                _this.livesLost++;
            }
        }

        // Controls & exit conditions
        _this.gameState.shouldStop = _this.livesLost >= 4 || _this.gameState.shouldStop;

        if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_ENTER))
            _this.gameState.shouldStop = true;

        if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_SPACE) && _this.subpixelPosition[_this.livesLost] == 174.)
        {
            _this.elapsedTimeSinceJump = 0;
            for (int i = _this.livesLost; i < 4; i++)
            {
                _this.commands[i] = true;
            }
        }

        // Delayed Jump for Dynos
        {
            _this.elapsedTimeSinceJump += deltaTime;

            float delayTable[4] = {0, .1, .2, .3};

            for (int i = _this.livesLost; i < 4; i++)
            {
                if (_this.elapsedTimeSinceJump >= delayTable[i - _this.livesLost] && _this.commands[i])
                {
                    _this.verticalSpeed[i] = _this.baseVerticalSpeed;
                    soundPlaySfx(_this.gameState.sound, SFX_HERO_JUMP);
                    _this.commands[i] = false;
                }
            }
        }

        // Set position from float to int (subpixel estimation)
        for (int i = 0; i < 4; i++)
        {
            _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + i].position.y = floor(_this.subpixelPosition[i]);
        }

        // gravity and velocity calculation
        for (int i = 0; i < 4; i++)
        {
            _this.verticalSpeed[i] += _this.gravity * deltaTime;
            _this.subpixelPosition[i] += _this.verticalSpeed[i] * deltaTime;
        }

        // collision with floor
        for (int i = _this.livesLost; i < 4; i++)
        {
            _this.subpixelPosition[i] = fminf(174., _this.subpixelPosition[i]);
        }

        // RENDER SECTION
        // Draw background clouds
        for (int i = 0; i < 4; i++)
        {
            _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i];
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i], _this.gameState.graphics.imageData);
            _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i] + _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i], _this.gameState.graphics.imageData);
        }

        // Draw Background
        {
            _this.screenPosition += backgroundSpeedFrame;
            _this.screenPosition = _this.screenPosition > -320. ? _this.screenPosition : 0.;
            _this.gameState.sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_BACKGROUND], _this.gameState.graphics.imageData);
            _this.gameState.sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition + 320;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_BACKGROUND], _this.gameState.graphics.imageData);
        }

        // Draw Obstacles
        for (int i = 0; i < matrixSize; i++)
        {
            if (obstacleStream[i] == 1)
            {
                _this.gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].position.x = _this.obstaclePosition + _this.gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * i;
                spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_OBSTACLE_1], _this.gameState.graphics.imageData);
            }
        }

        // Draw Heroes
        {
            _this.elapsedTimeSinceHit += deltaTime;
            int elapsedTimeSinceHitI = _this.elapsedTimeSinceHit;
            elapsedTimeSinceHitI = (_this.elapsedTimeSinceHit - elapsedTimeSinceHitI) * 100;
            float distanceBetweenDynos = _this.backgroundSpeed / _this.baseBackgroundSpeed;
            _this.gameState.sprites[ASSET_LEVEL2_HERO_BLUE].position.x = _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 10. * distanceBetweenDynos;
            _this.gameState.sprites[ASSET_LEVEL2_HERO_RED].position.x = _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 20. * distanceBetweenDynos;
            _this.gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].position.x = _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 30. * distanceBetweenDynos;

            for (int i = 3; i >= 0; i--)
            {
                _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + i].animation.frameRate = 15. * distanceBetweenDynos;
                _this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + i] = spriteDrawTransparentAnimatedClipped(_this.gameState.sprites[ASSET_LEVEL2_HERO_GREEN + i], _this.gameState.graphics.imageData, deltaTime);
            }
        }

        // Draw UI
        {
            graphicsDrawSquareFill(_this.gameState.graphics.imageData, (PointI){1, 1}, (PointI){(int)_this.runningDistance, 10}, (Color){0xFF, 0, 0});
            graphicsDrawSquare(_this.gameState.graphics.imageData, (PointI){1, 1}, (PointI){317, 10}, (Color){0xCC, 0xCC, 0xCC});
            _this.runningDistance += (-backgroundSpeedFrame / 100.) * 6;
            char stringToPrint[200] = {0};
            snprintf(stringToPrint, 200, "distance %.2f", _this.runningDistance);
            graphicsPrintString(_this.gameState.graphics.imageData, (PointI){120, 4}, stringToPrint, (Color){0, 0, 0});
        }

        swapBuffersPrintFPSPollEvents(_this.gameState.graphics, deltaTime);
    }
    _this.gameState.shouldStop = false;

    return _this;
}

Level2 level2GameCompleteLoop(Level2 _this)
{
    float UIPositionY = 1.;

    while (!_this.gameState.shouldStop && !_this.gameState.shouldQuit)
    {
        float deltaTime = getDeltaTime();
        _this.gameState.shouldQuit = isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_ESCAPE);

        // Clowds movement
        for (int i = 0; i < 4; i++)
        {
            if (_this.cloudPosition[i] < -_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x)
            {
                _this.cloudPosition[i] = 0.;
            }
        }

        // RENDER SECTION
        // Draw background clouds
        for (int i = 0; i < 4; i++)
        {
            _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i];
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i], _this.gameState.graphics.imageData);
            _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i] + _this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i], _this.gameState.graphics.imageData);
        }

        // Draw Background
        {
            _this.gameState.sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_BACKGROUND], _this.gameState.graphics.imageData);
            _this.gameState.sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition + 320;
            spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL2_BACKGROUND], _this.gameState.graphics.imageData);
        }

        // Draw UI
        {
            UIPositionY += 100. * deltaTime;

            if (UIPositionY > 120)
                _this.gameState.shouldStop = true;

            graphicsDrawSquareFill(_this.gameState.graphics.imageData, (PointI){1, 1 + UIPositionY}, (PointI){(int)_this.runningDistance, 10}, (Color){0xFF, 0, 0});
            graphicsDrawSquare(_this.gameState.graphics.imageData, (PointI){1, 1 + UIPositionY}, (PointI){317, 10}, (Color){0xCC, 0xCC, 0xCC});
            char stringToPrint[200] = {0};
            snprintf(stringToPrint, 200, "distance %.2f", _this.runningDistance);
            graphicsPrintString(_this.gameState.graphics.imageData, (PointI){120, 4 + UIPositionY}, stringToPrint, (Color){0, 0, 0});
        }

        swapBuffersPrintFPSPollEvents(_this.gameState.graphics, deltaTime);
    }
    _this.gameState.shouldStop = false;

    // Game Score Delay
    float elapsedTimeSinceScoreDisplay = 0;
    while (!(_this.gameState.shouldStop || _this.gameState.shouldQuit) && elapsedTimeSinceScoreDisplay < 2.)
    {
        float deltaTime = getDeltaTime();
        _this.gameState = gameStateCheckExitConditions(_this.gameState);

        elapsedTimeSinceScoreDisplay += deltaTime;
        glfwPollEvents();
    }

    return _this;
}

#define SIDE_LEFT 0b000000010
#define SIDE_RIGHT 0b000000001
#define SIDE_TOP 0b00000100
#define SIDE_BOTTOM 0b00001000

typedef struct
{
    PointI position, size;
    char sides;
} Tile;

#define TILE_SELECTED 0b00000010

#define TILES_CAPACITY 100

typedef enum
{
    LEVEL3_STATE_EDIT,
    LEVEL3_STATE_PLAYING,
    LEVEL3_STATE_EDIT_DRAWING,
    LEVEL3_STATE_COUNT
} Level3StateEnum;

typedef enum
{
    LEVEL3_HERO_STATE_IDLE,
    LEVEL3_HERO_STATE_WALING,
    LEVEL3_HERO_STATE_JUMPING,
    LEVEL3_HERO_STATE_COUNT
} Level3HeroState;

typedef struct
{
    Level3StateEnum state;
    int activeTile;
    float deltaTime;
    GameState gameState;
    struct
    {
        int capacity;
        int size;
        Tile data[TILES_CAPACITY];
    } tiles;
    struct
    {
        PointI origin, size;
    } newSquare;
    struct
    {
        PointI position;
        PointF positionF;
        PointF speed;
        Level3HeroState state;
        int spriteId;
        float gravity;
        float jumpSpeed;
        bool isWalking;
        bool isInFloor;
        bool isFlipped;
    } hero;
    bool showCollisions;
    PointI mousePos;
} Level3;

void level3DrawTile(Level3 _this, int tileId, char flags)
{
    if (!_this.showCollisions)
        return;
    Tile tile = _this.tiles.data[tileId];
    ImageData imageData = _this.gameState.graphics.imageData;
    PointI position = tile.position;
    PointI size = tile.size;

    if ((flags & TILE_SELECTED) == TILE_SELECTED)
        graphicsDrawSquareFill(imageData, position, size, (Color){0xFF, 0xFF, 0});
    else
        graphicsDrawSquare(imageData, position, size, (Color){0xFF, 0xFF, 0xFF});

    if ((tile.sides & SIDE_TOP) == SIDE_TOP)
        graphicsDrawLine(imageData, position, (PointI){position.x + size.x, position.y}, (Color){0xFF, 0, 0xFF});

    if ((tile.sides & SIDE_BOTTOM) == SIDE_BOTTOM)
        graphicsDrawLine(imageData, (PointI){position.x, position.y + size.y}, (PointI){position.x + size.x, position.y + size.y}, (Color){0xFF, 0, 0xFF});

    if ((tile.sides & SIDE_LEFT) == SIDE_LEFT)
        graphicsDrawLine(imageData, (PointI){position.x, position.y}, (PointI){position.x, position.y + size.y}, (Color){0xFF, 0, 0xFF});

    if ((tile.sides & SIDE_RIGHT) == SIDE_RIGHT)
        graphicsDrawLine(imageData, (PointI){position.x + size.x, position.y}, (PointI){position.x + size.x, position.y + size.y}, (Color){0xFF, 0, 0xFF});
}

Level3 level3Create()
{
    Level3 _this = {0};
    _this.hero.spriteId = ASSET_LEVEL3_HERO_IDLE;
    _this.hero.gravity = 1000.;
    _this.hero.positionF.x = 100.;
    _this.hero.positionF.y = 100.;
    _this.hero.speed.x = 100.;
    _this.hero.jumpSpeed = -350.;
    _this.activeTile = 0;
    _this.tiles.capacity = TILES_CAPACITY;
    _this.state = LEVEL3_STATE_PLAYING;
    return _this;
}

Level3 level3HandleCollisions(Level3 _this)
{
    int halfWide = 0;
    PointF lastPosition = _this.hero.positionF;

    _this.hero.isInFloor = false;

    if (_this.activeTile != -1)
    {
        Tile tile = _this.tiles.data[_this.activeTile];
        // restrict position
        if ((tile.sides & SIDE_LEFT) == SIDE_LEFT)
            _this.hero.positionF.x = fmax(tile.position.x + halfWide, _this.hero.positionF.x);

        if ((tile.sides & SIDE_RIGHT) == SIDE_RIGHT)
            _this.hero.positionF.x = fmin(tile.position.x + tile.size.x - halfWide, _this.hero.positionF.x);

        if ((tile.sides & SIDE_TOP) == SIDE_TOP)
            _this.hero.positionF.y = fmax(tile.position.y + halfWide, _this.hero.positionF.y);

        if ((tile.sides & SIDE_BOTTOM) == SIDE_BOTTOM)
            _this.hero.positionF.y = fmin(tile.position.y + tile.size.y - halfWide, _this.hero.positionF.y);
    }

    _this.activeTile = -1;
    for (int i = 0; i < _this.tiles.size; i++)
    {
        if (_this.hero.positionF.x >= _this.tiles.data[i].position.x &&
            _this.hero.positionF.x <= _this.tiles.data[i].position.x + _this.tiles.data[i].size.x &&
            _this.hero.positionF.y >= _this.tiles.data[i].position.y &&
            _this.hero.positionF.y <= _this.tiles.data[i].position.y + _this.tiles.data[i].size.y)
        {
            _this.activeTile = i;
            break;
        }
    }

    if (lastPosition.y != _this.hero.positionF.y)
    {
        _this.hero.isInFloor = true;
        _this.hero.speed.y = 0.;
    }

    return _this;
}

Level3 level3CreateCollisionMaps(Level3 _this)
{
#include "level3CollisionDef.txt"

    return _this;
}

Level3 level3MoveBackground(Level3 _this)
{
    float backgroundSpeed = 10.;
    static PointF positionF = {
        0.,
        -64.,
    };

    positionF.y += backgroundSpeed * _this.deltaTime;

    if (positionF.y > 0)
    {
        positionF.y = -64.;
    }
    _this.gameState.sprites[ASSET_LEVEL3_BACKGROUND_TILED].position.y = (int)positionF.y;

    return _this;
}

void level3SerializeLevelCollisions(Level3 _this)
{
    FILE *fp = fopen("src/level3CollisionDef.txt", "w+");

    fprintf(fp, "//-------\n");
    for (int i = 0; i < _this.tiles.size; i++)
    {
        Tile tile = _this.tiles.data[i];
        fprintf(fp, "_this.tiles.data[_this.tiles.size++] = (Tile){\n");
        fprintf(fp, ".position = (PointI){%d, %d},\n", tile.position.x, tile.position.y);
        fprintf(fp, ".size = (PointI){%d, %d},\n", tile.size.x, tile.size.y);
        fprintf(fp, ".sides = 0 ");
        if ((tile.sides & SIDE_LEFT) != 0)
        {
            fprintf(fp, "| ");
            fprintf(fp, "SIDE_LEFT ");
        }
        if ((tile.sides & SIDE_RIGHT) != 0)
        {
            fprintf(fp, "| ");
            fprintf(fp, "SIDE_RIGHT ");
        }
        if ((tile.sides & SIDE_TOP) != 0)
        {
            fprintf(fp, "| ");
            fprintf(fp, "SIDE_TOP ");
        }
        if ((tile.sides & SIDE_BOTTOM) != 0)
        {
            fprintf(fp, "| ");
            fprintf(fp, "SIDE_BOTTOM");
        }
        fprintf(fp, "}; \n\n ");
    }
    fclose(fp);
    printf("level saved\n");
}

Level3 level3GameLoop(Level3 _this)
{
    _this = level3CreateCollisionMaps(_this);

    while (!(_this.gameState.shouldStop || _this.gameState.shouldQuit))
    {
        _this.gameState = gameStateCheckExitConditions(_this.gameState);

        _this = level3MoveBackground(_this);
        _this.deltaTime = getDeltaTime();

        spriteDrawClipped(_this.gameState.sprites[ASSET_LEVEL3_BACKGROUND_TILED], _this.gameState.graphics.imageData);
        spriteDrawTransparentClipped(_this.gameState.sprites[ASSET_LEVEL3_BACKGROUND], _this.gameState.graphics.imageData);

        {
            double x, y;
            glfwGetCursorPos(_this.gameState.graphics.window, &x, &y);
            _this.mousePos = (PointI){fmax(fmin(x, 319), 0.), fmax(fmin(y, 239), 0.)};
        }
        switch (_this.state)
        {
        case LEVEL3_STATE_PLAYING:
        {
            _this.hero.isWalking = false;
            double lastPositionX = _this.hero.positionF.x;

            if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                _this.hero.positionF.x += _this.hero.speed.x * _this.deltaTime;

            if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
                _this.hero.positionF.x -= _this.hero.speed.x * _this.deltaTime;

            if (lastPositionX - _this.hero.positionF.x > 0)
                _this.hero.isFlipped = true;

            if (lastPositionX - _this.hero.positionF.x < 0)
                _this.hero.isFlipped = false;

            if (lastPositionX != _this.hero.positionF.x)
                _this.hero.isWalking = true;

            if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_E))
                _this.state = LEVEL3_STATE_EDIT;

            if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_H))
                _this.showCollisions = !_this.showCollisions;

            for (int i = 0; i < _this.tiles.size; i++)
            {
                if (_this.activeTile == i)
                    level3DrawTile(_this, i, TILE_SELECTED);
                else
                    level3DrawTile(_this, i, 0);
            }

            if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_SPACE) && _this.hero.isInFloor)
            {
                _this.hero.speed.y = _this.hero.jumpSpeed;
                soundPlaySfx(_this.gameState.sound, SFX_HERO_JUMP);
            }

            _this.hero.speed.y += _this.hero.gravity * _this.deltaTime;
            _this.hero.positionF.y += _this.hero.speed.y * _this.deltaTime;
            _this = level3HandleCollisions(_this);
        }
        break;
        case LEVEL3_STATE_EDIT_DRAWING:
            if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_TAB))
            {
                _this.state = LEVEL3_STATE_EDIT;

                double x, y;
                glfwGetCursorPos(_this.gameState.graphics.window, &x, &y);
                PointI size = (PointI){(int)x - _this.newSquare.origin.x, (int)y - _this.newSquare.origin.y};
                _this.tiles.data[_this.tiles.size++] = (Tile){.position = _this.newSquare.origin, .size = size, .sides = 0xff};
                _this.activeTile = _this.tiles.size - 1;
            }

            graphicsDrawSquare(_this.gameState.graphics.imageData, _this.newSquare.origin, (PointI){_this.mousePos.x - _this.newSquare.origin.x, _this.mousePos.y - _this.newSquare.origin.y}, (Color){0xff, 0, 0});
            graphicsDrawLine(_this.gameState.graphics.imageData, (PointI){0, _this.mousePos.y}, (PointI){319, _this.mousePos.y}, (Color){0xFF, 0xFF, 0xFF});
            graphicsDrawLine(_this.gameState.graphics.imageData, (PointI){_this.mousePos.x, 0}, (PointI){_this.mousePos.x, 239}, (Color){0xFF, 0xFF, 0xFF});
            graphicsPutPixel(_this.gameState.graphics.imageData, _this.mousePos, (Color){0xFF, 0, 0});

            for (int i = 0; i < _this.tiles.size; i++)
                level3DrawTile(_this, i, 0);

            break;
        case LEVEL3_STATE_EDIT:
            if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_TAB))
            {
                _this.state = LEVEL3_STATE_EDIT_DRAWING;
                double x, y;
                glfwGetCursorPos(_this.gameState.graphics.window, &x, &y);
                _this.newSquare.origin = (PointI){(int)x, (int)y};
            }

            if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                _this.hero.positionF.x += _this.hero.speed.x * _this.deltaTime;

            if (glfwGetKey(_this.gameState.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
                _this.hero.positionF.x -= _this.hero.speed.x * _this.deltaTime;

            if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_E))
                _this.state = LEVEL3_STATE_PLAYING;

            if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_H))
                _this.showCollisions = !_this.showCollisions;

            {
                bool activeTileAssigned = false;
                for (int i = 0; i < _this.tiles.size; i++)
                {
                    if (_this.mousePos.x >= _this.tiles.data[i].position.x &&
                        _this.mousePos.x <= _this.tiles.data[i].position.x + _this.tiles.data[i].size.x &&
                        _this.mousePos.y >= _this.tiles.data[i].position.y &&
                        _this.mousePos.y <= _this.tiles.data[i].position.y + _this.tiles.data[i].size.y)
                    {
                        _this.activeTile = i;
                        activeTileAssigned = true;
                        break;
                    }
                }
                if (!activeTileAssigned)
                    _this.activeTile = -1;
            }

            for (int i = 0; i < _this.tiles.size; i++)
            {
                if (_this.activeTile == i)
                {
                    if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_R))
                    {
                        for (int j = _this.activeTile; j < _this.tiles.size; j++)
                        {
                            _this.tiles.data[j] = _this.tiles.data[j + 1];
                        }
                        _this.tiles.size--;
                        _this.activeTile = -1;
                    }

                    if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_A))
                        _this.tiles.data[i].sides ^= SIDE_LEFT;

                    if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_D))
                        _this.tiles.data[i].sides ^= SIDE_RIGHT;

                    if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_W))
                        _this.tiles.data[i].sides ^= SIDE_TOP;

                    if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_S))
                        _this.tiles.data[i].sides ^= SIDE_BOTTOM;

                    level3DrawTile(_this, i, TILE_SELECTED);
                }
                else
                    level3DrawTile(_this, i, 0);
            }
            graphicsDrawLine(_this.gameState.graphics.imageData, (PointI){0, _this.mousePos.y}, (PointI){319, _this.mousePos.y}, (Color){0xFF, 0xFF, 0xFF});
            graphicsDrawLine(_this.gameState.graphics.imageData, (PointI){_this.mousePos.x, 0}, (PointI){_this.mousePos.x, 239}, (Color){0xFF, 0xFF, 0xFF});
            graphicsPutPixel(_this.gameState.graphics.imageData, _this.mousePos, (Color){0xFF, 0, 0});
            break;
        }

        if (isKeyJustPressed(_this.gameState.graphics.window, GLFW_KEY_Q))
            level3SerializeLevelCollisions(_this);

        _this.hero.spriteId = ASSET_LEVEL3_HERO_IDLE;

        if (_this.hero.isWalking)
        {
            _this.hero.spriteId = ASSET_LEVEL3_HERO_RUN;
        }

        if (!_this.hero.isInFloor)
        {
            if (_this.hero.speed.y < 0)
                _this.hero.spriteId = ASSET_LEVEL3_HERO_JUMP;
            else
                _this.hero.spriteId = ASSET_LEVEL3_HERO_FALL;
        }

        _this.gameState.sprites[_this.hero.spriteId].position.x = _this.hero.positionF.x;
        _this.gameState.sprites[_this.hero.spriteId].position.y = _this.hero.positionF.y;

        _this.gameState.sprites[_this.hero.spriteId].isFlipped = _this.hero.isFlipped;

        if (_this.gameState.sprites[_this.hero.spriteId].animated)
            _this.gameState.sprites[_this.hero.spriteId] = spriteDrawTransparentAnimatedClipped(_this.gameState.sprites[_this.hero.spriteId], _this.gameState.graphics.imageData, _this.deltaTime);
        else
            spriteDrawTransparentClipped(_this.gameState.sprites[_this.hero.spriteId], _this.gameState.graphics.imageData);

        swapBuffersPrintFPSPollEvents(_this.gameState.graphics, _this.deltaTime);
    }

    return _this;
}

int main(void)
{
    // Init memory, load assets and general initialization
    staticAllocatorInit(2878340 * 2);

    GameState gameState = {0};
    gameState.graphics = graphicsCreate(320, 240, false);
    gameState.sound = soundCreate();

    loadAssets(gameState.sprites);
    Soloud_setGlobalVolume(gameState.sound.soloud, 1.);

    // Run levels one after another
    // ============================
    // Level1
    // ============================
    if (0)
    {
        soundPlaySpeech(gameState.sound, SPEECH_SHOOT_THE_BAD_GUYS);
        Level1 _this = level1Create();
        _this.gameState = gameState;
        initDeltaTime();
        level1InitPositions(_this.positions);
        _this = level1InitEnemies(_this);
        _this = level1Tutorial(_this);
        _this = level1GameLoop(_this);
        _this = level1GameCompleteLoop(_this);

        if (_this.gameState.shouldQuit)
            goto Cleanup;
    }

    // ============================
    // Level2
    // ============================
    if (0)
    {
        soundPlaySpeech(gameState.sound, SPEECH_JUMP_THE_ROCKS);
        Level2 _this = level2Create();
        _this.gameState = gameState;
        _this = level2TutorialLoop(_this);
        _this = level2GameLoop(_this);
        _this = level2GameCompleteLoop(_this);

        if (_this.gameState.shouldQuit)
            goto Cleanup;
    }

    // ============================
    // Level3
    // ============================
    {
        Level3 _this = level3Create();
        _this.gameState = gameState;
        _this = level3GameLoop(_this);

        if (gameState.shouldQuit)
            goto Cleanup;
    }

// Cleanup
Cleanup:
    graphicsDestroy(gameState.graphics);
    staticAllocatorDestroy();
    return 0;
}