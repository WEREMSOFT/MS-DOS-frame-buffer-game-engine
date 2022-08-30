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

PointI ENEMY_OFFSET;

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

    ASSET_LEVEL2_HERO_GREEEN,
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

typedef struct
{
    PointI textPosition;
    QuadrantPosition quadPosition;
    Sound sound;
    Enemy enemies[8];
    PointI positions[QPOS_COUNT];
    int enemiesRemaining;
    int enemiesKilled;
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

    _this[ASSET_LEVEL2_HERO_GREEEN] = spriteCreate("assets/level2/heroGreen.bmp");
    _this[ASSET_LEVEL2_HERO_RED] = spriteCreate("assets/level2/heroRed.bmp");
    _this[ASSET_LEVEL2_HERO_BLUE] = spriteCreate("assets/level2/heroBlue.bmp");
    _this[ASSET_LEVEL2_HERO_YELLOW] = spriteCreate("assets/level2/heroYellow.bmp");

    _this[ASSET_LEVEL2_OBSTACLE_1] = spriteCreate("assets/level2/tree1.bmp");
    _this[ASSET_LEVEL2_OBSTACLE_2] = spriteCreate("assets/level2/tree2.bmp");
    _this[ASSET_LEVEL2_OBSTACLE_3] = spriteCreate("assets/level2/tree3.bmp");
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

static Level1 level1InitEnemies(Level1 _this, GameState gameState)
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
        _this.enemies[i].position.x = _this.enemies[i].basePosition.x = (int)_this.positions[i].x + ENEMY_OFFSET.x;
        _this.enemies[i].position.y = _this.enemies[i].basePosition.y = (int)_this.positions[i].y + ENEMY_OFFSET.y;
        _this.enemies[i] = enemyPassToStateHidden(_this.enemies[i]);

        if (i <= QPOS_TOP_RIGHT)
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_SMALL;
        else if (i >= QPOS_LEFT && i <= QPOS_RIGHT)
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_MEDIUM;
        else
            _this.enemies[i].spriteId = ASSET_ENEMY_GREEN_BIG;

        _this.enemies[i].bottomOffset = _this.enemies[i].lowerClippingPosition;
        _this.enemies[i].topLimit = _this.enemies[i].lowerClippingPosition - gameState.sprites[_this.enemies[i].spriteId].size.y;
        _this.enemies[i].speedMultiplicator = (float)gameState.sprites[_this.enemies[i].spriteId].size.y / (float)gameState.sprites[ASSET_ENEMY_GREEN_BIG].size.y;
    }

    return _this;
}

GameState level1MainLoop(GameState gameState)
{
    // Initialization
    Level1 _this = {0};

    level1InitPositions(_this.positions);
    gameState = gameState;

    ENEMY_OFFSET.y = 0;
    ENEMY_OFFSET.x = -8;

    _this = level1InitEnemies(_this, gameState);

    for (int i = 0; i < 8; i++)
    {
        _this.enemies[i] = enemyPassToStateHidden(_this.enemies[i]);
    }

    soundPlaySpeech(gameState.sound, SPEECH_SHOOT_THE_BAD_GUYS);
    double elapsedTime = glfwGetTime();
    double gameElapsedTime = 0;
    double enemySpeed = 100.;

    for (int i = 0; i < 8; i++)
    {
        _this.enemies[i] = enemyPassToStateGoingUp(_this.enemies[i]);
    }

    // Tutorial Loop
    while (!gameState.shouldStop && !gameState.shouldQuit)
    {
        int hiddenEnemies = 0;
        float dt = getDeltaTime();
        gameState.shouldQuit = isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ESCAPE);

        enemyProcessStateGoingDown(_this.enemies, dt, enemySpeed, NULL, gameState.sound);
        enemyProcessStateGoingUp(_this.enemies, dt, enemySpeed);

        for (int i = 0; i < 8; i++)
        {
            if (_this.enemies[i].state == ENEMY_STATE_HIDDEN)
                hiddenEnemies++;
        }

        if (hiddenEnemies == 8)
            gameState.shouldStop = true;

        // DRAW
        // Draw Background
        spriteDrawClipped(gameState.sprites[ASSET_BACKGROUND], gameState.graphics.imageData);
        spriteDrawClipped(gameState.sprites[ASSET_TOP_SCORE_SQUARE], gameState.graphics.imageData);
        // Draw Enemies
        for (int i = 0; i < 8; i++)
        {
            gameState.sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
            gameState.sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;
            spriteDrawTransparentClippedLowerLine(gameState.sprites[_this.enemies[i].spriteId], gameState.graphics.imageData, _this.enemies[i].lowerClippingPosition);
        };

        spriteDrawTransparentClipped(gameState.sprites[ASSET_FOREGROUND], gameState.graphics.imageData);
        spriteDrawTransparentAnimatedClipped(&gameState.sprites[ASSET_HOW_TO_PLAY], gameState.graphics.imageData, dt);
        printFPS(gameState.graphics, dt);
        graphicsSwapBuffers(gameState.graphics);
        glfwPollEvents();
    }

    gameState.shouldStop = false;
    char *enemiesRemaining[100] = {0};
    char *enemiesKilled[100] = {0};
    char *percentageKilled[100] = {0};
    _this.enemiesRemaining = 99;

    // Game
    while (!gameState.shouldStop && !gameState.shouldQuit && _this.enemiesRemaining > 0)
    {
        float dt = getDeltaTime();

        gameState.shouldQuit = isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ESCAPE);
        if (isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ENTER))
            gameState.shouldStop = true;

        // Enemy selection and trigger to attack
        if (glfwGetTime() - elapsedTime > .5)
        {
            elapsedTime = glfwGetTime();
            int enemyToDisplay = rand() % 8;

            if (_this.enemies[enemyToDisplay].state == ENEMY_STATE_HIDDEN)
                _this.enemies[enemyToDisplay] = enemyPassToStateGoingUp(_this.enemies[enemyToDisplay]);
        }

        // Rise the difficulty by ramping the speed of the enemies
        gameElapsedTime += dt;

        enemySpeed = fminf(100 + gameElapsedTime, 300);

        enemyProcessStateGoingDown(_this.enemies, dt, enemySpeed, &_this.enemiesRemaining, gameState.sound);
        enemyProcessStateGoingUp(_this.enemies, dt, enemySpeed);
        enemyProcessStateDead(_this.enemies, dt);
        // Handle Controls
        if (glfwGetKey(gameState.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
            glfwGetKey(gameState.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP_LEFT];
            _this.quadPosition = QPOS_TOP_LEFT;
            goto continueControls;
        }

        if (glfwGetKey(gameState.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
            glfwGetKey(gameState.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP_RIGHT];
            _this.quadPosition = QPOS_TOP_RIGHT;
            goto continueControls;
        }

        if (glfwGetKey(gameState.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
            glfwGetKey(gameState.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM_LEFT];
            _this.quadPosition = QPOS_BOTTOM_LEFT;
            goto continueControls;
        }

        if (glfwGetKey(gameState.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
            glfwGetKey(gameState.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM_RIGHT];
            _this.quadPosition = QPOS_BOTTOM_RIGHT;
            goto continueControls;
        }

        if (glfwGetKey(gameState.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP];
            _this.quadPosition = QPOS_TOP;
            goto continueControls;
        }

        if (glfwGetKey(gameState.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM];
            _this.quadPosition = QPOS_BOTTOM;
            goto continueControls;
        }

        if (glfwGetKey(gameState.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_RIGHT];
            _this.quadPosition = QPOS_RIGHT;
            goto continueControls;
        }

        if (glfwGetKey(gameState.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            gameState.sprites[ASSET_SIGHT].position = _this.positions[QPOS_LEFT];
            _this.quadPosition = QPOS_LEFT;
            goto continueControls;
        }
        else
            _this.quadPosition = QPOS_NONE;
    continueControls:
        // DRAW

        // Draw Background
        spriteDrawClipped(gameState.sprites[ASSET_BACKGROUND], gameState.graphics.imageData);
        spriteDrawClipped(gameState.sprites[ASSET_TOP_SCORE_SQUARE], gameState.graphics.imageData);

        // Draw Sight
        spriteDrawTransparentClipped(gameState.sprites[ASSET_SIGHT], gameState.graphics.imageData);

        // Draw Enemies
        for (int i = 0; i < 8; i++)
        {
            if (_this.enemies[i].state == ENEMY_STATE_HIDDEN || !_this.enemies[i].visible)
                continue;

            gameState.sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
            gameState.sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;

            spriteDrawTransparentClippedLowerLine(gameState.sprites[_this.enemies[i].spriteId], gameState.graphics.imageData, _this.enemies[i].lowerClippingPosition);
        };

        if (gameState.sprites[ASSET_SHOOT].animation.isPlaying)
        {
            spriteDrawTransparentAnimatedClipped(&gameState.sprites[ASSET_SHOOT], gameState.graphics.imageData, dt);
        }
        else if (glfwGetKey(gameState.graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {

            if (_this.quadPosition != QPOS_NONE)
            {
                gameState.sprites[ASSET_SHOOT].position = gameState.sprites[ASSET_SIGHT].position;
                gameState.sprites[ASSET_SHOOT].position.x -= gameState.sprites[ASSET_SIGHT].size.x / 2;
                gameState.sprites[ASSET_SHOOT].position.y -= gameState.sprites[ASSET_SIGHT].size.y / 2;

                spriteDrawTransparentAnimatedClipped(&gameState.sprites[ASSET_SHOOT], gameState.graphics.imageData, dt);

                if (_this.enemies[_this.quadPosition].state != ENEMY_STATE_HIDDEN && _this.enemies[_this.quadPosition].state != ENEMY_STATE_DEAD)
                {
                    soundPlaySpeech(gameState.sound, SPEECH_NOOO);
                    _this.enemiesRemaining--;
                    _this.enemiesKilled++;
                    _this.enemies[_this.quadPosition] = enemyPassToStateDead(_this.enemies[_this.quadPosition]);
                }
                soundPlaySfx(gameState.sound, SFX_SHOOT_HERO);
            }
        }

        spriteDrawTransparentClipped(gameState.sprites[ASSET_FOREGROUND], gameState.graphics.imageData);
        printFPS(gameState.graphics, dt);
        snprintf(enemiesRemaining, 100, "enemies remaining: %d", _this.enemiesRemaining);
        snprintf(enemiesKilled, 100, "enemies killed: %d", _this.enemiesKilled);
        float enemiesKilledPercentage = _this.enemiesKilled / (100. - _this.enemiesRemaining) * 100.;
        snprintf(percentageKilled, 100, "hit percentage: %.0f%%", enemiesKilledPercentage);
        graphicsPrintString(gameState.graphics.imageData, (PointI){100, 10}, enemiesRemaining, (Color){0xFF, 0xFF, 0xFF});
        graphicsPrintString(gameState.graphics.imageData, (PointI){100, 20}, enemiesKilled, (Color){0xFF, 0xFF, 0xFF});
        graphicsPrintString(gameState.graphics.imageData, (PointI){100, 30}, percentageKilled, (Color){0xFF, 0xFF, 0xFF});
        graphicsSwapBuffers(gameState.graphics);
        glfwPollEvents();
    }
    gameState.shouldStop = false;
    // Level Complete Loop
    float statisticsOffset = 0.;
    while (!gameState.shouldStop && !gameState.shouldQuit)
    {
        float statisticsSpeed = 100.;
        float dt = getDeltaTime();
        gameState.shouldQuit = isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ESCAPE);

        if (isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ENTER))
            gameState.shouldStop = true;

        spriteDrawClipped(gameState.sprites[ASSET_BACKGROUND], gameState.graphics.imageData);
        // Draw Enemies
        for (int i = 0; i < 8; i++)
        {
            gameState.sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
            gameState.sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;

            spriteDrawTransparentClippedLowerLine(gameState.sprites[_this.enemies[i].spriteId], gameState.graphics.imageData, _this.enemies[i].lowerClippingPosition);
        };

        spriteDrawTransparentClipped(gameState.sprites[ASSET_FOREGROUND], gameState.graphics.imageData);
        printFPS(gameState.graphics, dt);

        statisticsOffset = fminf(statisticsOffset + statisticsSpeed * dt, 100.);

        if (statisticsOffset == 100.)
        {
            gameState.shouldStop = true;
        }

        gameState.sprites[ASSET_TOP_SCORE_SQUARE].position.y = statisticsOffset;
        spriteDrawClipped(gameState.sprites[ASSET_TOP_SCORE_SQUARE], gameState.graphics.imageData);
        graphicsPrintString(gameState.graphics.imageData, (PointI){100, 20 + statisticsOffset}, enemiesKilled, (Color){0xFF, 0xFF, 0xFF});
        graphicsPrintString(gameState.graphics.imageData, (PointI){100, 30 + statisticsOffset}, percentageKilled, (Color){0xFF, 0xFF, 0xFF});
        graphicsSwapBuffers(gameState.graphics);
        glfwPollEvents();
    }

    // Game Score Delay
    gameState.shouldStop = false;
    float elapsedTimeSinceScoreDisplay = 0;
    while (!gameState.shouldStop && !gameState.shouldQuit && elapsedTimeSinceScoreDisplay < 2.)
    {
        float deltaTime = getDeltaTime();
        gameState.shouldQuit = isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ESCAPE);
        if (isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ENTER))
            gameState.shouldStop = true;

        elapsedTimeSinceScoreDisplay += deltaTime;
        glfwPollEvents();
    }
    gameState.shouldStop = true;
    return gameState;
}

GameState level2MainLoop(GameState gameState)
{

    gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].position.x = 80;
    gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].position.y = 174;
    gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].animated = true;
    gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].animation.frameCount = 6;
    gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].animation.frameWidth = 24;
    gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].animation.frameRate = 15;

    gameState.sprites[ASSET_LEVEL2_HERO_BLUE].position.x = 70;
    gameState.sprites[ASSET_LEVEL2_HERO_BLUE].position.y = 174;
    gameState.sprites[ASSET_LEVEL2_HERO_BLUE].animated = true;
    gameState.sprites[ASSET_LEVEL2_HERO_BLUE].animation.frameCount = 6;
    gameState.sprites[ASSET_LEVEL2_HERO_BLUE].animation.frameWidth = 24;
    gameState.sprites[ASSET_LEVEL2_HERO_BLUE].animation.frameRate = 15;

    gameState.sprites[ASSET_LEVEL2_HERO_RED].position.x = 60;
    gameState.sprites[ASSET_LEVEL2_HERO_RED].position.y = 174;
    gameState.sprites[ASSET_LEVEL2_HERO_RED].animated = true;
    gameState.sprites[ASSET_LEVEL2_HERO_RED].animation.frameCount = 6;
    gameState.sprites[ASSET_LEVEL2_HERO_RED].animation.frameWidth = 24;
    gameState.sprites[ASSET_LEVEL2_HERO_RED].animation.frameRate = 15;

    gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].position.x = 50;
    gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].position.y = 174;
    gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].animated = true;
    gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].animation.frameCount = 6;
    gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].animation.frameWidth = 24;
    gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].animation.frameRate = 15;

    gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].position.y = 179;

#define VERTICAL_SPEED -300.
#define BACKGROUND_SPEED -250.

    float backgroundSpeed = BACKGROUND_SPEED;
    float backgroundAcceleration = -5.0;
    bool commands[4] = {0};
    float gravity = 1000.;
    float subpixelPosition[4] = {174., 174., 174., 174.};
    float screenPosition = 0.;
    float verticalSpeed[4] = {0};
    float elapsedTimeSinceJump = 0;
    float elapsedTimeSinceHit = 10000;

    float clowdSpeedRatio[] = {.05,
                               .1,
                               .2,
                               .22};

    float cloudPosition[] = {0.,
                             0.,
                             0.,
                             0.};
    float obstaclePosition = 320.;
    bool collided = false;
    int livesLost = 0;
    float elapsedTimeBlink = 0;
    double runningDistance = 0;

    while (!gameState.shouldStop && !gameState.shouldQuit)
    {
        float deltaTime = getDeltaTime();
        gameState.shouldQuit = isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ESCAPE);

        backgroundSpeed += backgroundAcceleration * deltaTime;
        float backgroundSpeedFrame = deltaTime * backgroundSpeed;

        // Clowds movement
        for (int i = 0; i < 4; i++)
        {
            cloudPosition[i] += backgroundSpeedFrame * clowdSpeedRatio[i];
            if (cloudPosition[i] < -gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x)
            {
                cloudPosition[i] = 0.;
            }
        }

        static char obstacleStream[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1};
        static int matrixSize = sizeof(obstacleStream) / sizeof(obstacleStream[0]);

        // Obstacles & collision
        {
            collided = false;
            obstaclePosition += backgroundSpeedFrame;

            if (obstaclePosition < -gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * matrixSize)
                obstaclePosition = 320. + gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x;

            PointI distance;
            for (int i = 0; i < matrixSize; i++)
            {
                if (obstacleStream[i] == 1)
                {
                    static PointI obstaclePosV = {0, 174.};
                    obstaclePosV.x = obstaclePosition + gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * i;
                    distance.x = obstaclePosV.x + gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x / 2 - gameState.sprites[ASSET_LEVEL2_HERO_GREEEN + livesLost].position.x;
                    distance.y = obstaclePosV.y - gameState.sprites[ASSET_LEVEL2_HERO_GREEEN + livesLost].position.y;
                    float distanceScalar = distance.x * distance.x + distance.y * distance.y;
                    if (50 > distanceScalar)
                    {
                        collided = true;
                    }
                }
            }

            if (collided && elapsedTimeSinceHit > 1.)
            {
                elapsedTimeSinceHit = 0;
                soundPlaySfx(gameState.sound, SFX_HERO_HURT);
                backgroundSpeed = BACKGROUND_SPEED;
                verticalSpeed[livesLost] = VERTICAL_SPEED;
                livesLost++;
            }
        }

        // Controls & exit conditions
        gameState.shouldStop = livesLost >= 4;

        if (isKeyJustPressed(gameState.graphics.window, GLFW_KEY_ENTER))
            gameState.shouldStop = true;

        if (isKeyJustPressed(gameState.graphics.window, GLFW_KEY_SPACE) && subpixelPosition[livesLost] == 174.)
        {
            elapsedTimeSinceJump = 0;
            for (int i = livesLost; i < 4; i++)
            {
                commands[i] = true;
            }
        }

        // Delayed Jump for Dynos
        {
            elapsedTimeSinceJump += deltaTime;

            float delayTable[4] = {0, .1, .2, .3};

            for (int i = livesLost; i < 4; i++)
            {
                if (elapsedTimeSinceJump >= delayTable[i - livesLost] && commands[i])
                {
                    verticalSpeed[i] = VERTICAL_SPEED;
                    soundPlaySfx(gameState.sound, SFX_HERO_JUMP);
                    commands[i] = false;
                }
            }
        }

        // Set position from float to int (subpixel estimation)
        for (int i = 0; i < 4; i++)
        {
            gameState.sprites[ASSET_LEVEL2_HERO_GREEEN + i].position.y = floor(subpixelPosition[i]);
        }

        // gravity and velocity calculation
        for (int i = 0; i < 4; i++)
        {
            verticalSpeed[i] += gravity * deltaTime;
            subpixelPosition[i] += verticalSpeed[i] * deltaTime;
        }

        // collision with floor
        for (int i = livesLost; i < 4; i++)
        {
            subpixelPosition[i] = fminf(174., subpixelPosition[i]);
        }

        // RENDER SECTION
        // Draw background clouds
        for (int i = 0; i < 4; i++)
        {
            gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = cloudPosition[i];
            spriteDrawTransparentClipped(gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i], gameState.graphics.imageData);
            gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = cloudPosition[i] + gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x;
            spriteDrawTransparentClipped(gameState.sprites[ASSET_LEVEL2_CLOUD_1 + i], gameState.graphics.imageData);
        }

        printFPS(gameState.graphics, deltaTime);

        // Draw Background
        {
            screenPosition += backgroundSpeedFrame;
            screenPosition = screenPosition > -320. ? screenPosition : 0.;
            gameState.sprites[ASSET_LEVEL2_BACKGROUND].position.x = screenPosition;
            spriteDrawTransparentClipped(gameState.sprites[ASSET_LEVEL2_BACKGROUND], gameState.graphics.imageData);
            gameState.sprites[ASSET_LEVEL2_BACKGROUND].position.x = screenPosition + 320;
            spriteDrawTransparentClipped(gameState.sprites[ASSET_LEVEL2_BACKGROUND], gameState.graphics.imageData);
        }

        // Draw Obstacles
        for (int i = 0; i < matrixSize; i++)
        {
            if (obstacleStream[i] == 1)
            {
                gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].position.x = obstaclePosition + gameState.sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * i;
                spriteDrawTransparentClipped(gameState.sprites[ASSET_LEVEL2_OBSTACLE_1], gameState.graphics.imageData);
            }
        }

        // Draw Heroes
        {
            elapsedTimeSinceHit += deltaTime;
            int elapsedTimeSinceHitI = elapsedTimeSinceHit;
            elapsedTimeSinceHitI = (elapsedTimeSinceHit - elapsedTimeSinceHitI) * 100;
            float distanceBetweenDynos = backgroundSpeed / BACKGROUND_SPEED;
            gameState.sprites[ASSET_LEVEL2_HERO_BLUE].position.x = gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].position.x - 10. * distanceBetweenDynos;
            gameState.sprites[ASSET_LEVEL2_HERO_RED].position.x = gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].position.x - 20. * distanceBetweenDynos;
            gameState.sprites[ASSET_LEVEL2_HERO_YELLOW].position.x = gameState.sprites[ASSET_LEVEL2_HERO_GREEEN].position.x - 30. * distanceBetweenDynos;
            for (int i = 3; i >= 0; i--)
            {
                spriteDrawTransparentAnimatedClipped(&gameState.sprites[ASSET_LEVEL2_HERO_GREEEN + i], gameState.graphics.imageData, deltaTime);
            }
        }

        // Draw UI
        {
            runningDistance += -backgroundSpeedFrame / 100.;
            char stringToPrint[200] = {0};
            snprintf(stringToPrint, 200, "distance %.2f", runningDistance);
            graphicsPrintString(gameState.graphics.imageData, (PointI){0, 0}, stringToPrint, (Color){0xFF, 0, 0});
        }

        graphicsSwapBuffers(gameState.graphics);
        glfwPollEvents();
    }
    return gameState;
}

int main(void)
{
    // Init memory, load assets and general initialization
    staticAllocatorInit(100092024);

    Program _this = {0};
    _this.gameState.graphics = graphicsCreate(320, 240, true);
    _this.gameState.sound = soundCreate();

    loadAssets(_this.gameState.sprites);
    Soloud_setGlobalVolume(_this.gameState.sound.soloud, 1.);

    // Run levels one after another
    _this.gameState = level1MainLoop(_this.gameState);
    if (_this.gameState.shouldQuit)
        goto Cleanup;

    _this.gameState.shouldStop = false;

    _this.gameState = level2MainLoop(_this.gameState);
    if (_this.gameState.shouldQuit)
        goto Cleanup;

// Cleanup
Cleanup:
    graphicsDestroy(_this.gameState.graphics);
    staticAllocatorDestroy();
    return 0;
}