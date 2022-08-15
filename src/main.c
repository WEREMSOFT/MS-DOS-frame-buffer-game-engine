#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    SFX_BLIP,
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
    ASSET_SIGHT,
    ASSET_SHOOT,

    ASSET_ENEMY_GREEN_BIG,
    ASSET_ENEMY_GREEN_MEDIUM,
    ASSET_ENEMY_GREEN_SMALL,

    ASSET_ENEMY_GREEN_BIG_SHOOT,
    ASSET_ENEMY_GREEN_MEDIUM_SHOOT,
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
    int topLimit;
    int bottomOffset;
    bool visible;
    float elapsedStateTime;
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

void enemyProcessStateGoingDown(Enemy *enemies, float deltaTime, double enemySpeed)
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
        enemies[i].position.y += enemySpeed * deltaTime;
    }
}

void enemyProcessStateGoingUp(Enemy *enemies, float deltaTime, double enemySpeed)
{
    for (int i = 0; i < 8; i++)
    {
        if (enemies[i].state != ENEMY_STATE_GOING_UP)
            continue;
        if (enemies[i].position.y < enemies[i].topLimit)
        {
            enemies[i] = enemyPassToStateGoingDown(enemies[i]);
            continue;
        }
        enemies[i].position.y -= enemySpeed * deltaTime;
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
        _this.enemies[i].topLimit = _this.enemies[i].lowerClippingPosition - _this.sprites[_this.enemies[i].spriteId].size.y;
    }

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

    level1InitPositions(_this.positions);

    _this.sound = sound;
    _this.graphics = graphics;
    for (int i = 0; i < ASSET_COUNT; i++)
    {
        _this.sprites[i] = sprites[i];
    }

    ENEMY_OFFSET.y = 0;
    ENEMY_OFFSET.x = -8;

    _this.shouldQuit = false;

    _this = level1InitEnemies(_this);

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
        if (_this.enemies[i].state == ENEMY_STATE_HIDDEN || !_this.enemies[i].visible)
            continue;

        _this.sprites[_this.enemies[i].spriteId].position.x = (int)_this.enemies[i].position.x;
        _this.sprites[_this.enemies[i].spriteId].position.y = (int)_this.enemies[i].position.y;

        spriteDrawTransparentClippedLowerLine(_this.sprites[_this.enemies[i].spriteId], _this.graphics.imageData, _this.enemies[i].lowerClippingPosition);
    };
}

Level1 level1Update(Level1 _this)
{
    soundPlaySpeech(_this.sound, SPEECH_SHOOT_THE_BAD_GUYS);
    bool shouldContinue = true;
    double elapsedTime = glfwGetTime();
    double gameElapsedTime = 0;
    double enemySpeed = 100.;
    while (shouldContinue && !_this.shouldQuit)
    {
        float dt = getDeltaTime();

        _this.shouldQuit = isKeyJustPressed(_this.graphics.window, GLFW_KEY_ESCAPE);
        spriteDrawClipped(_this.sprites[ASSET_BACKGROUND], _this.graphics.imageData);

        // Enemy selection and trigger to attack
        if (glfwGetTime() - elapsedTime > 1.)
        {
            elapsedTime = glfwGetTime();
            int enemyToDisplay = rand() % 8;

            if (_this.enemies[enemyToDisplay].state == ENEMY_STATE_HIDDEN)
                _this.enemies[enemyToDisplay] = enemyPassToStateGoingUp(_this.enemies[enemyToDisplay]);
        }
        // Rise the difficulty by ramping the speed of the enemies
        gameElapsedTime += dt;

        enemySpeed = fminf(100 + gameElapsedTime, 175);

        enemyProcessStateGoingDown(_this.enemies, dt, enemySpeed);
        enemyProcessStateGoingUp(_this.enemies, dt, enemySpeed);
        enemyProcessStateDead(_this.enemies, dt);

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

                if (_this.enemies[_this.quadPosition].state != ENEMY_STATE_HIDDEN && _this.enemies[_this.quadPosition].state != ENEMY_STATE_DEAD)
                {
                    soundPlaySpeech(_this.sound, SPEECH_NOOO);
                    _this.enemies[_this.quadPosition] = enemyPassToStateDead(_this.enemies[_this.quadPosition]);
                }
                soundPlaySfx(_this.sound, SFX_SHOOT_HERO);
            }
        }

        spriteDrawTransparentClipped(_this.sprites[ASSET_FOREGROUND], _this.graphics.imageData);
        printFPS(_this.graphics, dt);
        graphicsSwapBuffers(_this.graphics);
        glfwPollEvents();
    }

    return _this;
}

Program programCreate()
{
    staticAllocatorInit(100092024);
    Program _this = {0};
    _this.graphics = graphicsCreate(320, 240);
    _this.sound = soundCreate();
    loadAssets(_this.sprites);
    Soloud_setGlobalVolume(_this.sound.soloud, 1.);
    _this.mainMenu = level1Create(_this.graphics, _this.sprites, _this.sound);

    return _this;
}

void programMainLoop(Program _this)
{
    while (!_this.mainMenu.shouldQuit)
    {
        _this.mainMenu = level1Update(_this.mainMenu);
        if (_this.mainMenu.shouldQuit)
            return;
    }
}

void programDestroy(Program _this)
{
    graphicsDestroy(_this.graphics);
    staticAllocatorDestroy();
}

int main(void)
{

    Program _this = programCreate();
    programMainLoop(_this);
    programDestroy(_this);

    return 0;
}