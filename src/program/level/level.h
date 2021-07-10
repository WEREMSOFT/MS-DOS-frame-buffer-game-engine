#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "../core/graphics/graphics.h"
#include "../core/sprite/sprite.h"
#include "../sound/sound.h"
#include "../enemy/enemy.h"
#include "../core/array/array.h"
#include <pthread.h>

#define MAX_HERO_BULLETS_ON_SCREEN 10

typedef struct
{
    double x, y, z;
} Star;

enum ProjectilesEnum
{
    PROJECTILE_HERO,
    PROJECTILE_COUNT
};

enum EnemiesEnum
{
    ENEMY_SHOOTING_EGG,
    ENEMY_COUNT
};

typedef struct
{
    Graphics graphics;
    Sprite enemyLayer;
    Sprite projectiles[PROJECTILE_COUNT];
    PointF heroBulletsPositions[MAX_HERO_BULLETS_ON_SCREEN];
    Sprite hero;
    Sprite thrusters;
    Sprite enemy;
    Sound sound;
    pthread_mutex_t lock;
    bool shouldQuit;
} Level;

typedef struct
{
    Graphics graphics;
    Array *enemiesArray;
    Sprite *enemyLayer;
    ImageData screen;
    bool shouldQuit;
    int from;
    int to;
    pthread_mutex_t *lock;
} PthreadInfo;

Level levelCreate(Graphics graphics, Sprite *sprites, Sound sound);
Level levelMainLoop(Level this);
void levelDestroy(Level this);

#endif
