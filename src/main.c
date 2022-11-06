#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <soloud_c.h>
#define UR_SCREEN_WIDTH 320
#define UR_SCREEN_HEIGHT 240

#define __STATIC_ALLOC_IMPLEMENTATION__
#include "program/core/stackAllocator/staticAlloc.h"

#define UR_MALLOC allocStatic
#define UR_REALLOC reallocStatic
#define UR_FREE freeStatic

#define UR_PUT_PIXEL urPutPixel
#include "universal_renderer.h"
#include "program/core/utils/utils.h"

#include "program/core/input/keyboard.h"

#define S3L_RESOLUTION_X 320
#define S3L_RESOLUTION_Y 240
#define S3L_PIXEL_FUNCTION drawPixel
#include <small3Dlib/small3dlib.h>

Graphics *globalGraphics;

void urPutPixel(URPointI point, URColor color)
{
    int position = (point.x + point.y * UR_SCREEN_WIDTH) % globalGraphics->imageData.bufferSize;
    globalGraphics->imageData.data[position] = (Color){color.r, color.g, color.b};
}

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

    urPutPixel((URPointI){(int)p->x, p->y}, color);
}

#define TILES_CAPACITY 100

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

struct GameState;

typedef struct
{
    struct GameState *gameState;
    PointI textPosition;
    QuadrantPosition quadPosition;
    Sound sound;
    float enemySpeed;
    Enemy enemies[8];
    URPointI positions[QPOS_COUNT];
    int enemiesRemaining;
    int enemiesKilled;
    char enemiesRemainingString[100];
    char enemiesKilledString[100];
    char percentageKilledString[100];
    double elapsedTime;
    double gameElapsedTime;
    PointI enemyOffset;
    int hiddenEnemies;
    float statisticsOffset;
} Level1;

typedef struct
{
    struct GameState *gameState;
    float backgroundSpeed;
    char obstacleStream[32];
    int matrixSize;
    float backgroundSpeedFrame;
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
    float runningDistance;
    float UIPositionY;
} Level2;

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
    URRectI rectangle;
    char sides;
} Tile;

typedef struct
{
    struct GameState *gameState;
    Level3StateEnum state;
    int activeTile;
    struct
    {
        int capacity;
        int size;
        Tile data[TILES_CAPACITY];
    } tiles;
    struct
    {
        URPointI origin, size;
    } newSquare;
    struct
    {
        URRectI collisionBox;
        URPointI position;
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
    URPointI mousePos;
} Level3;

typedef struct
{
    struct GameState *gameState;
    S3L_Unit cubeVertices[S3L_CUBE_VERTEX_COUNT * 3];
    S3L_Index cubeTriangles[S3L_CUBE_TRIANGLE_COUNT * 3];
    S3L_Model3D cubeModel;
    S3L_Scene scene;
    float cubeRotation[3];
    float cameraRotation[3];
} Level4;

typedef enum
{
    GAME_STATE_CLICK_TO_START,
    GAME_STATE_LEVEL1_INIT,
    GAME_STATE_LEVEL1_TUTORIAL,
    GAME_STATE_LEVEL1_PLAY,
    GAME_STATE_LEVEL1_EXIT,

    GAME_STATE_LEVEL2_INIT,
    GAME_STATE_LEVEL2_TUTORIAL,
    GAME_STATE_LEVEL2_PLAY,
    GAME_STATE_LEVEL2_EXIT,

    GAME_STATE_LEVEL3_INIT,
    GAME_STATE_LEVEL3_PLAY,

    GAME_STATE_LEVEL4_INIT,
    GAME_STATE_LEVEL4_PLAY,

    GAME_STATE_COUNT
} GameStateEnum;

typedef struct GameState
{
    struct GameState *_self;
    Graphics graphics;
    URSprite sprites[ASSET_COUNT];
    Sound sound;
    float deltaTime;
    bool shouldQuit;
    bool shouldStop;
    GameStateEnum gameStateEnum;
    Level1 level1;
    Level2 level2;
    Level3 level3;
    Level4 level4;
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

void loadAssets(URSprite *_this)
{
    _this[ASSET_BACKGROUND] = urSpriteCreate("assets/background.bmp");
    _this[ASSET_FOREGROUND] = urSpriteCreate("assets/foreground.bmp");
    _this[ASSET_TOP_SCORE_SQUARE] = urSpriteCreate("assets/topScoreSquare.bmp");
    _this[ASSET_TOP_SCORE_SQUARE].position.x = 32;

    _this[ASSET_HOW_TO_PLAY] = urSpriteCreate("assets/howToPlay.bmp");
    _this[ASSET_HOW_TO_PLAY].position.x = 119;
    _this[ASSET_HOW_TO_PLAY].position.y = 130;
    _this[ASSET_HOW_TO_PLAY].animated = true;
    _this[ASSET_HOW_TO_PLAY].animation.frameCount = 5;
    _this[ASSET_HOW_TO_PLAY].animation.frameRate = 5;
    _this[ASSET_HOW_TO_PLAY].animation.frameWidth = 82;

    _this[ASSET_SIGHT] = urSpriteCreate("assets/aimcross.bmp");

    _this[ASSET_SHOOT] = urSpriteCreate("assets/shoot.bmp");
    _this[ASSET_SHOOT].animated = true;
    _this[ASSET_SHOOT].animation.frameCount = 4;
    _this[ASSET_SHOOT].animation.currentFrame = 0;
    _this[ASSET_SHOOT].animation.frameWidth = 55;
    _this[ASSET_SHOOT].animation.frameRate = 10.;

    _this[ASSET_ENEMY_GREEN_BIG] = urSpriteCreate("assets/enemyGreenBig2.bmp");
    _this[ASSET_ENEMY_GREEN_BIG_SHOOT] = urSpriteCreate("assets/enemyGreenBig1.bmp");

    _this[ASSET_ENEMY_GREEN_MEDIUM] = urSpriteCreate("assets/enemyGreenSmall2.bmp");
    _this[ASSET_ENEMY_GREEN_MEDIUM_SHOOT] = urSpriteCreate("assets/enemyGreenSmall1.bmp");

    _this[ASSET_ENEMY_GREEN_SMALL] = urSpriteCreate("assets/enemyGreenTiny2.bmp");
    _this[ASSET_ENEMY_GREEN_SMALL_SHOOT] = urSpriteCreate("assets/enemyGreenTiny1.bmp");

    _this[ASSET_LEVEL2_BACKGROUND] = urSpriteCreate("assets/level2/background.bmp");
    _this[ASSET_LEVEL2_CLOUD_1] = urSpriteCreate("assets/level2/cloud3.bmp");
    _this[ASSET_LEVEL2_CLOUD_2] = urSpriteCreate("assets/level2/cloud2.bmp");
    _this[ASSET_LEVEL2_CLOUD_3] = urSpriteCreate("assets/level2/cloud1.bmp");
    _this[ASSET_LEVEL2_CLOUD_4] = urSpriteCreate("assets/level2/cloud0.bmp");

    _this[ASSET_LEVEL2_HERO_GREEN] = urSpriteCreate("assets/level2/heroGreen.bmp");
    _this[ASSET_LEVEL2_HERO_RED] = urSpriteCreate("assets/level2/heroRed.bmp");
    _this[ASSET_LEVEL2_HERO_BLUE] = urSpriteCreate("assets/level2/heroBlue.bmp");
    _this[ASSET_LEVEL2_HERO_YELLOW] = urSpriteCreate("assets/level2/heroYellow.bmp");

    _this[ASSET_LEVEL2_OBSTACLE_1] = urSpriteCreate("assets/level2/tree1.bmp");
    _this[ASSET_LEVEL2_OBSTACLE_2] = urSpriteCreate("assets/level2/tree2.bmp");
    _this[ASSET_LEVEL2_OBSTACLE_3] = urSpriteCreate("assets/level2/tree3.bmp");

    _this[ASSET_LEVEL2_HOW_TO_PLAY] = urSpriteCreate("assets/level2/howToPlay.bmp");
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

    _this[ASSET_LEVEL3_BACKGROUND] = urSpriteCreate("assets/level3/background.bmp");
    _this[ASSET_LEVEL3_BACKGROUND_TILED] = urSpriteCreate("assets/level3/background-tiled.bmp");

    _this[ASSET_LEVEL3_HERO_JUMP] = urSpriteCreate("assets/level3/playerJump.bmp");
    _this[ASSET_LEVEL3_HERO_JUMP].center.x = -16;
    _this[ASSET_LEVEL3_HERO_JUMP].center.y = -32;

    _this[ASSET_LEVEL3_HERO_FALL] = urSpriteCreate("assets/level3/playerFall.bmp");
    _this[ASSET_LEVEL3_HERO_FALL].center.x = -16;
    _this[ASSET_LEVEL3_HERO_FALL].center.y = -32;

    _this[ASSET_LEVEL3_HERO_RUN] = urSpriteCreate("assets/level3/playerRun.bmp");
    _this[ASSET_LEVEL3_HERO_RUN].position.x = 0;
    _this[ASSET_LEVEL3_HERO_RUN].position.y = 0;
    _this[ASSET_LEVEL3_HERO_RUN].animated = true;
    _this[ASSET_LEVEL3_HERO_RUN].animation.frameCount = 12;
    _this[ASSET_LEVEL3_HERO_RUN].animation.frameWidth = 32;
    _this[ASSET_LEVEL3_HERO_RUN].animation.frameRate = 15;
    _this[ASSET_LEVEL3_HERO_RUN].center.x = -16;
    _this[ASSET_LEVEL3_HERO_RUN].center.y = -32;

    _this[ASSET_LEVEL3_HERO_IDLE] = urSpriteCreate("assets/level3/playerIdle.bmp");
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
        _this.enemies[i].topLimit = _this.enemies[i].lowerClippingPosition - _this.gameState->sprites[_this.enemies[i].spriteId].size.y;
        _this.enemies[i].speedMultiplicator = (float)_this.gameState->sprites[_this.enemies[i].spriteId].size.y / (float)_this.gameState->sprites[ASSET_ENEMY_GREEN_BIG].size.y;
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
    enemyProcessStateGoingDownTutorial(_this.enemies, _this.gameState->deltaTime, _this.enemySpeed);
    enemyProcessStateGoingUp(_this.enemies, _this.gameState->deltaTime, _this.enemySpeed);

    for (int i = 0; i < 8; i++)
    {
        if (_this.enemies[i].state == ENEMY_STATE_HIDDEN)
            _this.hiddenEnemies++;
    }

    if (_this.hiddenEnemies >= 8)
    {
        _this.gameState->shouldStop = true;
    }
    // DRAW
    // Draw Background
    urSpriteDrawClipped(_this.gameState->sprites[ASSET_BACKGROUND]);
    urSpriteDrawClipped(_this.gameState->sprites[ASSET_TOP_SCORE_SQUARE]);
    // Draw Enemies
    for (int i = 0; i < 8; i++)
    {
        _this.gameState->sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
        _this.gameState->sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;
        urSpriteDrawTransparentClippedLowerLine(_this.gameState->sprites[_this.enemies[i].spriteId], _this.enemies[i].lowerClippingPosition);
    };

    urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_FOREGROUND]);
    _this.gameState->sprites[ASSET_HOW_TO_PLAY] = urSpriteDrawTransparentAnimatedClipped(_this.gameState->sprites[ASSET_HOW_TO_PLAY], _this.gameState->deltaTime);
    return _this;
}

Level1 level1HandleControls(Level1 _this)
{
    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.gameState->sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP_LEFT];
        _this.quadPosition = QPOS_TOP_LEFT;
        return _this;
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.gameState->sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP_RIGHT];
        _this.quadPosition = QPOS_TOP_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS &&
        glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.gameState->sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM_LEFT];
        _this.quadPosition = QPOS_BOTTOM_LEFT;
        return _this;
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS &&
        glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.gameState->sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM_RIGHT];
        _this.quadPosition = QPOS_BOTTOM_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        _this.gameState->sprites[ASSET_SIGHT].position = _this.positions[QPOS_TOP];
        _this.quadPosition = QPOS_TOP;
        return _this;
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        _this.gameState->sprites[ASSET_SIGHT].position = _this.positions[QPOS_BOTTOM];
        _this.quadPosition = QPOS_BOTTOM;
        return _this;
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        _this.gameState->sprites[ASSET_SIGHT].position = _this.positions[QPOS_RIGHT];
        _this.quadPosition = QPOS_RIGHT;
        return _this;
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        _this.gameState->sprites[ASSET_SIGHT].position = _this.positions[QPOS_LEFT];
        _this.quadPosition = QPOS_LEFT;
        return _this;
    }

    _this.quadPosition = QPOS_NONE;
    return _this;
}

Level1 level1Update(Level1 _this)
{
    // Enemy selection and trigger to attack
    if (glfwGetTime() - _this.elapsedTime > .5)
    {
        _this.elapsedTime = glfwGetTime();
        int enemyToDisplay = rand() % 8;

        if (_this.enemies[enemyToDisplay].state == ENEMY_STATE_HIDDEN)
            _this.enemies[enemyToDisplay] = enemyPassToStateGoingUp(_this.enemies[enemyToDisplay]);
    }

    // Rise the difficulty by ramping the speed of the enemies
    _this.gameElapsedTime += _this.gameState->deltaTime;

    _this.enemySpeed = fminf(100 + _this.gameElapsedTime, 300);

    enemyProcessStateGoingDown(_this.enemies, _this.gameState->deltaTime, _this.enemySpeed, &_this.enemiesRemaining, _this.gameState->sound);
    enemyProcessStateGoingUp(_this.enemies, _this.gameState->deltaTime, _this.enemySpeed);
    enemyProcessStateDead(_this.enemies, _this.gameState->deltaTime);
    // Handle Controls
    _this = level1HandleControls(_this);
    // DRAW
    // Draw Background
    urSpriteDrawClipped(_this.gameState->sprites[ASSET_BACKGROUND]);
    urSpriteDrawClipped(_this.gameState->sprites[ASSET_TOP_SCORE_SQUARE]);

    // Draw Enemies
    for (int i = 0; i < 8; i++)
    {
        if (_this.enemies[i].state == ENEMY_STATE_HIDDEN || !_this.enemies[i].visible)
            continue;

        _this.gameState->sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
        _this.gameState->sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;

        urSpriteDrawTransparentClippedLowerLine(_this.gameState->sprites[_this.enemies[i].spriteId], _this.enemies[i].lowerClippingPosition);
    };

    if (_this.gameState->sprites[ASSET_SHOOT].animation.isPlaying)
    {
        _this.gameState->sprites[ASSET_SHOOT] = urSpriteDrawTransparentAnimatedClipped(_this.gameState->sprites[ASSET_SHOOT], _this.gameState->deltaTime);
    }
    else if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {

        if (_this.quadPosition != QPOS_NONE)
        {
            _this.gameState->sprites[ASSET_SHOOT].position = _this.gameState->sprites[ASSET_SIGHT].position;
            _this.gameState->sprites[ASSET_SHOOT].position.x -= _this.gameState->sprites[ASSET_SIGHT].size.x / 2;
            _this.gameState->sprites[ASSET_SHOOT].position.y -= _this.gameState->sprites[ASSET_SIGHT].size.y / 2;

            _this.gameState->sprites[ASSET_SHOOT] = urSpriteDrawTransparentAnimatedClipped(_this.gameState->sprites[ASSET_SHOOT], _this.gameState->deltaTime);

            if (_this.enemies[_this.quadPosition].state != ENEMY_STATE_HIDDEN && _this.enemies[_this.quadPosition].state != ENEMY_STATE_DEAD)
            {
                soundPlaySpeech(_this.gameState->sound, SPEECH_NOOO);
                _this.enemiesRemaining--;
                _this.enemiesKilled++;
                _this.enemies[_this.quadPosition] = enemyPassToStateDead(_this.enemies[_this.quadPosition]);
            }
            soundPlaySfx(_this.gameState->sound, SFX_SHOOT_HERO);
        }
    }

    urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_FOREGROUND]);
    // Draw Sight
    if (_this.quadPosition != QPOS_NONE)
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_SIGHT]);

    snprintf(_this.enemiesRemainingString, 100, "enemies remaining: %d", _this.enemiesRemaining);
    snprintf(_this.enemiesKilledString, 100, "enemies killed: %d", _this.enemiesKilled);
    float enemiesKilledPercentage = _this.enemiesKilled / (100. - _this.enemiesRemaining) * 100.;
    snprintf(_this.percentageKilledString, 100, "hit percentage: %.0f%%", enemiesKilledPercentage);
    urPrintString((URPointI){100, 10}, _this.enemiesRemainingString, (URColor){0xFF, 0xFF, 0xFF});
    urPrintString((URPointI){100, 20}, _this.enemiesKilledString, (URColor){0xFF, 0xFF, 0xFF});
    urPrintString((URPointI){100, 30}, _this.percentageKilledString, (URColor){0xFF, 0xFF, 0xFF});

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
    float statisticsSpeed = 100.;
    urSpriteDrawClipped(_this.gameState->sprites[ASSET_BACKGROUND]);
    // Draw Enemies
    for (int i = 0; i < 8; i++)
    {
        _this.gameState->sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
        _this.gameState->sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;

        urSpriteDrawTransparentClippedLowerLine(_this.gameState->sprites[_this.enemies[i].spriteId], _this.enemies[i].lowerClippingPosition);
    };

    urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_FOREGROUND]);

    _this.statisticsOffset = fminf(_this.statisticsOffset + statisticsSpeed * _this.gameState->deltaTime, 100.);

    if (_this.statisticsOffset == 100.)
    {
        _this.gameState->shouldStop = true;
    }

    _this.gameState->sprites[ASSET_TOP_SCORE_SQUARE].position.y = _this.statisticsOffset;
    urSpriteDrawClipped(_this.gameState->sprites[ASSET_TOP_SCORE_SQUARE]);
    urPrintString((URPointI){100, 20 + _this.statisticsOffset}, _this.enemiesKilledString, (URColor){0xFF, 0xFF, 0xFF});
    urPrintString((URPointI){100, 30 + _this.statisticsOffset}, _this.percentageKilledString, (URColor){0xFF, 0xFF, 0xFF});

    // // Game Score Delay
    // _this.gameState->shouldStop = false;
    // float elapsedTimeSinceScoreDisplay = 0;
    // while (!_this.gameState->shouldStop && !_this.gameState->shouldQuit && elapsedTimeSinceScoreDisplay < 2.)
    // {
    //     float deltaTime = getDeltaTime();
    //     *_this.gameState = gameStateCheckExitConditions(*_this.gameState);

    //     elapsedTimeSinceScoreDisplay += deltaTime;
    //     glfwPollEvents();
    // }
    // _this.gameState->shouldStop = true;
    return _this;
}

Level2 level2Create()
{
    Level2 _this = {0};
    _this.UIPositionY = 1.;
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

    _this.matrixSize = 32;
    _this.obstacleStream[16] = 1;
    _this.obstacleStream[17] = 1;
    _this.obstacleStream[25] = 1;
    _this.obstacleStream[26] = 1;
    _this.obstacleStream[27] = 1;
    _this.obstacleStream[31] = 1;

    return _this;
}

Level2 level2TutorialLoop(Level2 _this)
{

    float backgroundSpeedFrame = _this.gameState->deltaTime * _this.backgroundSpeed;

    // Clowds movement
    for (int i = 0; i < 4; i++)
    {
        _this.cloudPosition[i] += backgroundSpeedFrame * _this.clowdSpeedRatio[i];
        if (_this.cloudPosition[i] < -_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x)
        {
            _this.cloudPosition[i] = 0.;
        }
    }

    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_SPACE) && _this.subpixelPosition[_this.livesLost] == 174.)
    {
        _this.gameState->shouldStop = true;

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
        _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i];
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i]);
        _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i] + _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i]);
    }

    // Draw Background
    {
        _this.screenPosition += backgroundSpeedFrame;
        _this.screenPosition = _this.screenPosition > -320. ? _this.screenPosition : 0.;
        _this.gameState->sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_BACKGROUND]);
        _this.gameState->sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition + 320;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_BACKGROUND]);
    }

    // Draw Heroes
    {
        _this.elapsedTimeSinceHit += _this.gameState->deltaTime;
        int elapsedTimeSinceHitI = _this.elapsedTimeSinceHit;
        elapsedTimeSinceHitI = (_this.elapsedTimeSinceHit - elapsedTimeSinceHitI) * 100;
        float distanceBetweenDynos = _this.backgroundSpeed / _this.baseBackgroundSpeed;
        _this.gameState->sprites[ASSET_LEVEL2_HERO_BLUE].position.x = _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 10. * distanceBetweenDynos;
        _this.gameState->sprites[ASSET_LEVEL2_HERO_RED].position.x = _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 20. * distanceBetweenDynos;
        _this.gameState->sprites[ASSET_LEVEL2_HERO_YELLOW].position.x = _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 30. * distanceBetweenDynos;

        for (int i = 3; i >= 0; i--)
        {
            _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + i].animation.frameRate = 15. * distanceBetweenDynos;
            _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + i] = urSpriteDrawTransparentAnimatedClipped(_this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + i], _this.gameState->deltaTime);
        }
    }
    // Draw UI
    _this.gameState->sprites[ASSET_LEVEL2_HOW_TO_PLAY] = urSpriteDrawTransparentAnimatedClipped(_this.gameState->sprites[ASSET_LEVEL2_HOW_TO_PLAY], _this.gameState->deltaTime);

    return _this;
}

Level2 level2HandleObstaclesAndCollisions(Level2 _this)
{
    _this.collided = false;
    _this.obstaclePosition += _this.backgroundSpeedFrame;

    if (_this.obstaclePosition < -_this.gameState->sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * _this.matrixSize)
        _this.obstaclePosition = 320. + _this.gameState->sprites[ASSET_LEVEL2_OBSTACLE_1].size.x;

    PointI distance;
    for (int i = 0; i < _this.matrixSize; i++)
    {
        if (_this.obstacleStream[i] == 1)
        {
            static PointI obstaclePosV = {0, 174.};
            obstaclePosV.x = _this.obstaclePosition + _this.gameState->sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * i;
            distance.x = obstaclePosV.x +
                         _this.gameState->sprites[ASSET_LEVEL2_OBSTACLE_1].size.x / 2 -
                         _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + _this.livesLost].position.x -
                         _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + _this.livesLost].animation.frameWidth;
            distance.y = obstaclePosV.y - _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + _this.livesLost].position.y;
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
        soundPlaySfx(_this.gameState->sound, SFX_HERO_HURT);
        _this.backgroundSpeed = _this.baseBackgroundSpeed;
        _this.verticalSpeed[_this.livesLost] = _this.baseVerticalSpeed;
        _this.livesLost++;
    }

    return _this;
}

Level2 level2DelayedJumpForDinosHandler(Level2 _this)
{
    _this.elapsedTimeSinceJump += _this.gameState->deltaTime;

    float delayTable[4] = {0, .1, .2, .3};

    for (int i = _this.livesLost; i < 4; i++)
    {
        if (_this.elapsedTimeSinceJump >= delayTable[i - _this.livesLost] && _this.commands[i])
        {
            _this.verticalSpeed[i] = _this.baseVerticalSpeed;
            soundPlaySfx(_this.gameState->sound, SFX_HERO_JUMP);
            _this.commands[i] = false;
        }
    }
    return _this;
}

Level2 level2Update(Level2 _this)
{
    _this.backgroundSpeed += _this.backgroundAcceleration * _this.gameState->deltaTime;
    _this.backgroundSpeedFrame = _this.gameState->deltaTime * _this.backgroundSpeed;

    // Clowds movement
    for (int i = 0; i < 4; i++)
    {
        _this.cloudPosition[i] += _this.backgroundSpeedFrame * _this.clowdSpeedRatio[i];
        if (_this.cloudPosition[i] < -_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x)
        {
            _this.cloudPosition[i] = 0.;
        }
    }

    // Obstacles & collision
    _this = level2HandleObstaclesAndCollisions(_this);
    // Controls & exit conditions
    _this.gameState->shouldStop = _this.livesLost >= 4 || _this.gameState->shouldStop;

    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_SPACE) && _this.subpixelPosition[_this.livesLost] == 174.)
    {
        _this.elapsedTimeSinceJump = 0;
        for (int i = _this.livesLost; i < 4; i++)
        {
            _this.commands[i] = true;
        }
    }

    // Delayed Jump for Dynos
    _this = level2DelayedJumpForDinosHandler(_this);

    // Set position from float to int (subpixel estimation)
    for (int i = 0; i < 4; i++)
    {
        _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + i].position.y = floor(_this.subpixelPosition[i]);
    }

    // gravity and velocity calculation
    for (int i = 0; i < 4; i++)
    {
        _this.verticalSpeed[i] += _this.gravity * _this.gameState->deltaTime;
        _this.subpixelPosition[i] += _this.verticalSpeed[i] * _this.gameState->deltaTime;
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
        _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i];
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i]);
        _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i] + _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i]);
    }

    // Draw Background
    {
        _this.screenPosition += _this.backgroundSpeedFrame;
        _this.screenPosition = _this.screenPosition > -320. ? _this.screenPosition : 0.;
        _this.gameState->sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_BACKGROUND]);
        _this.gameState->sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition + 320;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_BACKGROUND]);
    }

    // Draw Obstacles
    for (int i = 0; i < _this.matrixSize; i++)
    {
        if (_this.obstacleStream[i] == 1)
        {
            _this.gameState->sprites[ASSET_LEVEL2_OBSTACLE_1].position.x = _this.obstaclePosition + _this.gameState->sprites[ASSET_LEVEL2_OBSTACLE_1].size.x * i;
            urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_OBSTACLE_1]);
        }
    }

    // Draw Heroes
    {
        _this.elapsedTimeSinceHit += _this.gameState->deltaTime;
        int elapsedTimeSinceHitI = _this.elapsedTimeSinceHit;
        elapsedTimeSinceHitI = (_this.elapsedTimeSinceHit - elapsedTimeSinceHitI) * 100;
        float distanceBetweenDynos = _this.backgroundSpeed / _this.baseBackgroundSpeed;
        _this.gameState->sprites[ASSET_LEVEL2_HERO_BLUE].position.x = _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 10. * distanceBetweenDynos;
        _this.gameState->sprites[ASSET_LEVEL2_HERO_RED].position.x = _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 20. * distanceBetweenDynos;
        _this.gameState->sprites[ASSET_LEVEL2_HERO_YELLOW].position.x = _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN].position.x - 30. * distanceBetweenDynos;

        for (int i = 3; i >= 0; i--)
        {
            _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + i].animation.frameRate = 15. * distanceBetweenDynos;
            _this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + i] = urSpriteDrawTransparentAnimatedClipped(_this.gameState->sprites[ASSET_LEVEL2_HERO_GREEN + i], _this.gameState->deltaTime);
        }
    }

    // Draw UI
    {
        urDrawSquareFill((URPointI){1, 1}, (URPointI){(int)_this.runningDistance, 10}, (URColor){0xFF, 0, 0});
        urDrawSquare((URPointI){1, 1}, (URPointI){317, 10}, (URColor){0xCC, 0xCC, 0xCC});
        _this.runningDistance += (-_this.backgroundSpeedFrame / 100.) * 6;

        if (_this.runningDistance > 318)
            _this.gameState->shouldStop = true;

        char stringToPrint[200] = {0};
        snprintf(stringToPrint, 200, "distance %.2f", _this.runningDistance);
        urPrintString((URPointI){120, 4}, stringToPrint, (URColor){0, 0, 0});
    }

    return _this;
}

Level2 level2GameCompleteLoop(Level2 _this)
{
    _this.gameState->shouldQuit = isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_ESCAPE);

    // Clowds movement
    for (int i = 0; i < 4; i++)
    {
        if (_this.cloudPosition[i] < -_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x)
        {
            _this.cloudPosition[i] = 0.;
        }
    }

    // RENDER SECTION
    // Draw background clouds
    for (int i = 0; i < 4; i++)
    {
        _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i];
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i]);
        _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].position.x = _this.cloudPosition[i] + _this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i].size.x;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_CLOUD_1 + i]);
    }

    // Draw Background
    {
        _this.gameState->sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_BACKGROUND]);
        _this.gameState->sprites[ASSET_LEVEL2_BACKGROUND].position.x = _this.screenPosition + 320;
        urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL2_BACKGROUND]);
    }

    // Draw UI
    {
        _this.UIPositionY += 100. * _this.gameState->deltaTime;

        if (_this.UIPositionY > 120)
            _this.gameState->shouldStop = true;

        urDrawSquareFill((URPointI){1, 1 + _this.UIPositionY}, (URPointI){(int)_this.runningDistance, 10}, (URColor){0xFF, 0, 0});
        urDrawSquare((URPointI){1, 1 + _this.UIPositionY}, (URPointI){317, 10}, (URColor){0xCC, 0xCC, 0xCC});
        char stringToPrint[200] = {0};
        snprintf(stringToPrint, 200, "distance %.2f", _this.runningDistance);
        urPrintString((URPointI){120, 4 + _this.UIPositionY}, stringToPrint, (URColor){0, 0, 0});
    }

    // Game Score Delay
    // float elapsedTimeSinceScoreDisplay = 0;
    // while (!(_this.gameState->shouldStop || _this.gameState->shouldQuit) && elapsedTimeSinceScoreDisplay < 2.)
    // {
    //     float deltaTime = getDeltaTime();
    //     *_this.gameState = gameStateCheckExitConditions(*_this.gameState);

    //     elapsedTimeSinceScoreDisplay += deltaTime;
    //     glfwPollEvents();
    // }

    return _this;
}

#define SIDE_LEFT 0b000000010
#define SIDE_RIGHT 0b000000001
#define SIDE_TOP 0b00000100
#define SIDE_BOTTOM 0b00001000
#define TILE_SELECTED 0b00000010

void level3DrawTile(Level3 _this, int tileId, char flags)
{
    if (!_this.showCollisions)
        return;
    Tile tile = _this.tiles.data[tileId];
    ImageData imageData = _this.gameState->graphics.imageData;
    URPointI position = tile.rectangle.position;
    URPointI size = tile.rectangle.size;

    if ((flags & TILE_SELECTED) == TILE_SELECTED)
        urDrawSquareFill(position, size, (URColor){0xFF, 0xFF, 0});
    else
        urDrawSquare(position, size, (URColor){0xFF, 0xFF, 0xFF});

    if ((tile.sides & SIDE_TOP) == SIDE_TOP)
        urDrawLine(position, (URPointI){position.x + size.x, position.y}, (URColor){0xFF, 0, 0xFF});

    if ((tile.sides & SIDE_BOTTOM) == SIDE_BOTTOM)
        urDrawLine((URPointI){position.x, position.y + size.y}, (URPointI){position.x + size.x, position.y + size.y}, (URColor){0xFF, 0, 0xFF});

    if ((tile.sides & SIDE_LEFT) == SIDE_LEFT)
        urDrawLine((URPointI){position.x, position.y}, (URPointI){position.x, position.y + size.y}, (URColor){0xFF, 0, 0xFF});

    if ((tile.sides & SIDE_RIGHT) == SIDE_RIGHT)
        urDrawLine((URPointI){position.x + size.x, position.y}, (URPointI){position.x + size.x, position.y + size.y}, (URColor){0xFF, 0, 0xFF});
}

Level3 level3Create()
{
    Level3 _this = {0};
    _this.hero.spriteId = ASSET_LEVEL3_HERO_IDLE;
    _this.hero.collisionBox = (URRectI){{-8, -20}, {16, 20}};
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

    positionF.y += backgroundSpeed * _this.gameState->deltaTime;

    if (positionF.y > 0)
    {
        positionF.y = -64.;
    }
    _this.gameState->sprites[ASSET_LEVEL3_BACKGROUND_TILED].position.y = (int)positionF.y;

    return _this;
}

Level3 level3HandleCollisionsReactions(Level3 _this)
{

    PointF lastPosition = _this.hero.positionF;

    _this.hero.isInFloor = false;

    if (_this.activeTile != -1)
    {
        Tile tile = _this.tiles.data[_this.activeTile];
        // restrict position
        if ((tile.sides & SIDE_LEFT) == SIDE_LEFT)
            _this.hero.positionF.x = fmax(tile.rectangle.position.x, _this.hero.positionF.x);

        if ((tile.sides & SIDE_RIGHT) == SIDE_RIGHT)
            _this.hero.positionF.x = fmin(tile.rectangle.position.x + tile.rectangle.size.x, _this.hero.positionF.x);

        if ((tile.sides & SIDE_TOP) == SIDE_TOP)
            _this.hero.positionF.y = fmax(tile.rectangle.position.y, _this.hero.positionF.y);

        if ((tile.sides & SIDE_BOTTOM) == SIDE_BOTTOM)
            _this.hero.positionF.y = fmin(tile.rectangle.position.y + tile.rectangle.size.y, _this.hero.positionF.y);
    }

    _this.activeTile = -1;
    for (int i = 0; i < _this.tiles.size; i++)
    {
        if(urHitTestPointRect((URPointI){_this.hero.positionF.x, _this.hero.positionF.y}, _this.tiles.data[i].rectangle))
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

Level3 level3ProcessStatePlaying(Level3 _this)
{
    _this.hero.isWalking = false;
    double lastPositionX = _this.hero.positionF.x;

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        _this.hero.positionF.x += _this.hero.speed.x * _this.gameState->deltaTime;

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        _this.hero.positionF.x -= _this.hero.speed.x * _this.gameState->deltaTime;

    if (lastPositionX - _this.hero.positionF.x > 0)
        _this.hero.isFlipped = true;

    if (lastPositionX - _this.hero.positionF.x < 0)
        _this.hero.isFlipped = false;

    if (lastPositionX != _this.hero.positionF.x)
        _this.hero.isWalking = true;

    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_E))
        _this.state = LEVEL3_STATE_EDIT;

    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_H))
        _this.showCollisions = !_this.showCollisions;

    for (int i = 0; i < _this.tiles.size; i++)
    {
        if (_this.activeTile == i)
            level3DrawTile(_this, i, TILE_SELECTED);
        else
            level3DrawTile(_this, i, 0);
    }

    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_SPACE) && _this.hero.isInFloor)
    {
        _this.hero.speed.y = _this.hero.jumpSpeed;
        soundPlaySfx(_this.gameState->sound, SFX_HERO_JUMP);
    }

    _this.hero.speed.y += _this.hero.gravity * _this.gameState->deltaTime;
    _this.hero.positionF.y += _this.hero.speed.y * _this.gameState->deltaTime;
    // Handle collisions
    _this = level3HandleCollisionsReactions(_this);

    return _this;
}

Level3 level3ProcesStateEditDrawing(Level3 _this)
{
    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_TAB))
    {
        _this.state = LEVEL3_STATE_EDIT;
        double x, y;
        glfwGetCursorPos(_this.gameState->graphics.window, &x, &y);
        URPointI size = (URPointI){(int)x - _this.newSquare.origin.x, (int)y - _this.newSquare.origin.y};
        _this.tiles.data[_this.tiles.size++] = (Tile){.rectangle.position = _this.newSquare.origin, .rectangle.size = size, .sides = 0xff};
        _this.activeTile = _this.tiles.size - 1;
    }

    urDrawSquare((URPointI){_this.newSquare.origin.x, _this.newSquare.origin.y}, (URPointI){_this.mousePos.x - _this.newSquare.origin.x, _this.mousePos.y - _this.newSquare.origin.y}, (URColor){0xff, 0, 0});
    urDrawLine((URPointI){0, _this.mousePos.y}, (URPointI){319, _this.mousePos.y}, (URColor){0xFF, 0xFF, 0xFF});
    urDrawLine((URPointI){_this.mousePos.x, 0}, (URPointI){_this.mousePos.x, 239}, (URColor){0xFF, 0xFF, 0xFF});
    urPutPixel((URPointI){_this.mousePos.x, _this.mousePos.y}, (URColor){0xFF, 0, 0});

    for (int i = 0; i < _this.tiles.size; i++)
        level3DrawTile(_this, i, 0);
    return _this;
}

Level3 level3ProcessStateEdit(Level3 _this)
{
    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_TAB))
    {
        _this.state = LEVEL3_STATE_EDIT_DRAWING;
        double x, y;
        glfwGetCursorPos(_this.gameState->graphics.window, &x, &y);
        _this.newSquare.origin = (URPointI){(int)x, (int)y};
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        _this.hero.positionF.x += _this.hero.speed.x * _this.gameState->deltaTime;

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        _this.hero.positionF.x -= _this.hero.speed.x * _this.gameState->deltaTime;

    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_E))
        _this.state = LEVEL3_STATE_PLAYING;

    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_H))
        _this.showCollisions = !_this.showCollisions;

    {
        bool activeTileAssigned = false;
        for (int i = 0; i < _this.tiles.size; i++)
        {
            if(urHitTestPointRect(_this.mousePos, _this.tiles.data[i].rectangle))
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
            if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_R))
            {
                for (int j = _this.activeTile; j < _this.tiles.size; j++)
                {
                    _this.tiles.data[j] = _this.tiles.data[j + 1];
                }
                _this.tiles.size--;
                _this.activeTile = -1;
            }

            if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_A))
                _this.tiles.data[i].sides ^= SIDE_LEFT;

            if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_D))
                _this.tiles.data[i].sides ^= SIDE_RIGHT;

            if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_W))
                _this.tiles.data[i].sides ^= SIDE_TOP;

            if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_S))
                _this.tiles.data[i].sides ^= SIDE_BOTTOM;

            level3DrawTile(_this, i, TILE_SELECTED);
        }
        else
            level3DrawTile(_this, i, 0);
    }
    urDrawLine((URPointI){0, _this.mousePos.y}, (URPointI){319, _this.mousePos.y}, (URColor){0xFF, 0xFF, 0xFF});
    urDrawLine((URPointI){_this.mousePos.x, 0}, (URPointI){_this.mousePos.x, 239}, (URColor){0xFF, 0xFF, 0xFF});
    return _this;
}

void level3SerializeLevelDesign(Level3 _this)
{
    FILE *fp = fopen("src/level3CollisionDef.txt", "w+");

    fprintf(fp, "//-------\n");
    for (int i = 0; i < _this.tiles.size; i++)
    {
        Tile tile = _this.tiles.data[i];
        fprintf(fp, "_this.tiles.data[_this.tiles.size++] = (Tile){\n");
        fprintf(fp, ".rectangle.position = (URPointI){%d, %d},\n", tile.rectangle.position.x, tile.rectangle.position.y);
        fprintf(fp, ".rectangle.size = (URPointI){%d, %d},\n", tile.rectangle.size.x, tile.rectangle.size.y);
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

Level3 level3SetHeroAnimation(Level3 _this)
{
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
    return _this;
}

Level3 level3HeroDraw(Level3 _this)
{
    _this.gameState->sprites[_this.hero.spriteId].position.x = _this.hero.positionF.x;
    _this.gameState->sprites[_this.hero.spriteId].position.y = _this.hero.positionF.y;

    _this.gameState->sprites[_this.hero.spriteId].isFlipped = _this.hero.isFlipped;

    if (_this.gameState->sprites[_this.hero.spriteId].animated)
        _this.gameState->sprites[_this.hero.spriteId] = urSpriteDrawTransparentAnimatedClipped(_this.gameState->sprites[_this.hero.spriteId], _this.gameState->deltaTime);
    else
        urSpriteDrawTransparentClipped(_this.gameState->sprites[_this.hero.spriteId]);

    urDrawSquare(
        (URPointI){
            _this.hero.collisionBox.position.x + _this.hero.positionF.x,
            _this.hero.collisionBox.position.y + _this.hero.positionF.y,
        },
        _this.hero.collisionBox.size, 
    (URColor){0xFF, 0, 0});
    return _this;
}

Level3 level3Update(Level3 _this)
{
    _this = level3MoveBackground(_this);

    urSpriteDrawClipped(_this.gameState->sprites[ASSET_LEVEL3_BACKGROUND_TILED]);
    urSpriteDrawTransparentClipped(_this.gameState->sprites[ASSET_LEVEL3_BACKGROUND]);

    // Get Mouse Position
    {
        double x, y;
        glfwGetCursorPos(_this.gameState->graphics.window, &x, &y);
        _this.mousePos = (URPointI){fmax(fmin(x, 319), 0.), fmax(fmin(y, 239), 0.)};
    }

    // Level state handling
    switch (_this.state)
    {
    case LEVEL3_STATE_PLAYING:
        _this = level3ProcessStatePlaying(_this);
        break;
    case LEVEL3_STATE_EDIT_DRAWING:
        _this = level3ProcesStateEditDrawing(_this);
        break;
    case LEVEL3_STATE_EDIT:
        _this = level3ProcessStateEdit(_this);
        break;
    }

    if (isKeyJustPressed(_this.gameState->graphics.window, GLFW_KEY_Q))
        level3SerializeLevelDesign(_this);

    _this = level3SetHeroAnimation(_this);

    // setup hero and draw it
    _this = level3HeroDraw(_this);
    return _this;
}

GameState gameStateClickToStart(GameState _this)
{
    static int gameColor = 0;
    static float elapsedTime = 0;
    graphicsClear(_this.graphics.imageData);
    URColor static textColor = {255, 255, 255};
    char *clickToStart = "click to start";

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

Level4 level4Create(Level4 *_this)
{
    S3L_Unit cubeVertices[] = {S3L_CUBE_VERTICES(S3L_F)};
    memcpy(_this->cubeVertices, cubeVertices, sizeof(cubeVertices) * sizeof(S3L_F));

    S3L_Index cubeTriangles[] = {S3L_CUBE_TRIANGLES};
    memcpy(_this->cubeTriangles, cubeTriangles, sizeof(cubeTriangles) * sizeof(S3L_Index));

    S3L_model3DInit(_this->cubeVertices, S3L_CUBE_VERTEX_COUNT, _this->cubeTriangles, S3L_CUBE_TRIANGLE_COUNT, &_this->cubeModel);
    S3L_sceneInit(&_this->cubeModel, 1, &_this->scene);

    _this->scene.camera.transform.translation.y = S3L_F / 2;
    _this->scene.camera.transform.translation.z = -2 * S3L_F;

    return *_this;
}

Level4 level4Update(Level4 _this)
{
    graphicsClear(_this.gameState->graphics.imageData);

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_LEFT))
    {
        _this.cameraRotation[1] += 100. * _this.gameState->deltaTime;
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_RIGHT))
    {
        _this.cameraRotation[1] -= 100. * _this.gameState->deltaTime;
    }
    S3L_Vec4 camF, camR;
    S3L_rotationToDirections(_this.scene.camera.transform.rotation, 10000. * _this.gameState->deltaTime, &camF, &camR, 0);

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_A))
    {
        S3L_vec3Sub(&_this.scene.camera.transform.translation, camR);
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_D))
    {
        S3L_vec3Add(&_this.scene.camera.transform.translation, camR);
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_W))
    {
        S3L_vec3Add(&_this.scene.camera.transform.translation, camF);
    }

    if (glfwGetKey(_this.gameState->graphics.window, GLFW_KEY_S))
    {
        S3L_vec3Sub(&_this.scene.camera.transform.translation, camF);
    }

    _this.scene.camera.transform.rotation.y = _this.cameraRotation[1];

    _this.cubeRotation[0] += 100. * _this.gameState->deltaTime;

    _this.cubeModel.transform.rotation.x = _this.cubeRotation[0];
    _this.cubeModel.transform.rotation.y = _this.cubeRotation[1];
    _this.cubeModel.transform.rotation.z = _this.cubeRotation[2];

    S3L_newFrame();
    S3L_drawScene(_this.scene);
    return _this;
}

GameState gameMainLoop(GameState gameState)
{
    globalGraphics = &gameState.graphics;
    gameState.deltaTime = getDeltaTime();
    gameState = gameStateCheckExitConditions(gameState);
    switch (gameState.gameStateEnum)
    {
    case GAME_STATE_CLICK_TO_START:
        gameState = gameStateClickToStart(gameState);
        break;
    case GAME_STATE_LEVEL1_INIT:
        Soloud_setGlobalVolume(gameState.sound.soloud, 1.);
        soundPlaySpeech(gameState.sound, SPEECH_SHOOT_THE_BAD_GUYS);
        gameState.level1 = level1Create();
        gameState.level1.gameState = &gameState;
        level1InitPositions(gameState.level1.positions);
        gameState.level1 = level1InitEnemies(gameState.level1);
        gameState.gameStateEnum++;
        initDeltaTime();
        break;
    case GAME_STATE_LEVEL1_TUTORIAL:
        gameState.level1 = level1Tutorial(gameState.level1);
        break;
    case GAME_STATE_LEVEL1_PLAY:
        gameState.level1 = level1Update(gameState.level1);
        break;
    case GAME_STATE_LEVEL1_EXIT:
        gameState.level1 = level1GameCompleteLoop(gameState.level1);
        break;
    case GAME_STATE_LEVEL2_INIT:
        soundPlaySpeech(gameState.sound, SPEECH_JUMP_THE_ROCKS);
        gameState.level2 = level2Create();
        gameState.level2.gameState = &gameState;
        gameState.gameStateEnum++;
        break;
    case GAME_STATE_LEVEL2_TUTORIAL:
        gameState.level2 = level2TutorialLoop(gameState.level2);
        break;
    case GAME_STATE_LEVEL2_PLAY:
        gameState.level2 = level2Update(gameState.level2);
        break;
    case GAME_STATE_LEVEL2_EXIT:
        gameState.level2 = level2GameCompleteLoop(gameState.level2);
        break;
    case GAME_STATE_LEVEL3_INIT:
        gameState.level3 = level3Create();
        gameState.level3.gameState = &gameState;
        gameState.level3 = level3CreateCollisionMaps(gameState.level3);
        gameState.gameStateEnum++;
        break;
    case GAME_STATE_LEVEL3_PLAY:
        gameState.level3 = level3Update(gameState.level3);
        break;
    case GAME_STATE_LEVEL4_INIT:
        gameState.level4 = level4Create(&gameState._self->level4);
        gameState.level4.gameState = &gameState;
        gameState.gameStateEnum++;
        break;
    case GAME_STATE_LEVEL4_PLAY:
        gameState.level4 = level4Update(gameState.level4);
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

#ifdef __EMSCRIPTEN__
GameState gameState = {0};

void emscriptenLoopHandler()
{
    gameState = gameMainLoop(gameState);
}
#endif

int main(void)
{
    // Init memory, load assets and general initialization
    staticAllocatorInit(2878340 * 2);
    initDeltaTime();

#ifndef __EMSCRIPTEN__
    GameState gameState = {0};
#endif

    gameState._self = &gameState;
    gameState.graphics = graphicsCreate(320, 240, false);
    loadAssets(gameState.sprites);

    gameState.sound = soundCreate();
    gameState.gameStateEnum = GAME_STATE_LEVEL3_INIT;

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