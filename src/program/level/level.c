#include <pthread.h>
#include "level.h"
#include "../assetManager/assetManager.h"
#include "levelStaticFunctions.h"
#include "../utils/utils.h"
#include "../core/input/keyboard.h"
#include "../core/array/array.h"
#include <stdlib.h>
#include <soloud_c.h>

#define PTHREAD_COUNT 7

Level levelCreate(Graphics graphics, Sprite *sprites, Sound sound)
{
    Level this = {0};
    this.graphics = graphics;
    this.sound = sound;
    this.hero = sprites[ASSET_SHIP_BLUE];
    this.projectiles[PROJECTILE_HERO] = sprites[ASSET_HERO_BULLET];
    this.thrusters = sprites[ASSET_SHIP_THRUSTERS];
    this.enemy = sprites[ASSET_ENEMY_SHOOTING_EGG];
    this.shouldQuit = false;
    return this;
}

typedef struct
{
    Array *enemiesArray;
    Graphics graphics;
    bool shouldQuit;
    int from;
    int to;
    pthread_mutex_t *lock;
    bool *mainThreadBussy;
} PthreadInfo;

void *updateEnemies(void *params)
{

    PthreadInfo *this = (PthreadInfo *)params;

    double lastUpdate = 0;

    double deltaTime = 0;

    Array *enemiesArray = this->enemiesArray;
    Graphics graphics = this->graphics;
    while (!this->shouldQuit)
    {
        if (*this->mainThreadBussy)
            continue;
        double actualTime = glfwGetTime();
        deltaTime = actualTime - lastUpdate;
        lastUpdate = actualTime;
        for (int i = this->from; i < this->to; i++)
        {
            Enemy *enemy = (Enemy *)arrayGetElementAt(enemiesArray, i);
            *enemy = enemyFlyingEggUpdateState(*enemy, deltaTime);
            enemy->sprite.position = enemy->movementDef.position;
            spriteDrawTransparentClipped(enemy->sprite, graphics.imageData);
        }
    }

    return NULL;
}

Level levelMainLoop(Level this)
{
    Graphics graphics = this.graphics;
#ifdef PTHREAD_COUNT
    PthreadInfo pthreadInfo[PTHREAD_COUNT] = {0};
    pthread_mutex_t lock;
    bool mainThreadBussy = false;

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthreadInfo[i].graphics = this.graphics;
        pthreadInfo[i].lock = &lock;
        pthreadInfo[i].mainThreadBussy = &mainThreadBussy;
    }
#endif

#define ENEMIES_COUNT 10000
    Array *enemiesArray = arrayCreate(ENEMIES_COUNT, sizeof(Enemy));

    for (int i = 0; i < ENEMIES_COUNT; i++)
    {
        Enemy enemy = enemyFlyingEggCreate(this.enemy);
        enemy.movementDef.center.y = (random() % (graphics.imageData.size.y));
        enemy.movementDef.position.x += random() % 100;
        enemy.movementDef.speed += random() % 100;
        enemy.movementDef.phase = random() % 10;
        arrayInsertElement(&enemiesArray, &enemy);
    }
#ifdef PTHREAD_COUNT
    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthreadInfo[i].enemiesArray = enemiesArray;
        pthreadInfo[i].from = i * ENEMIES_COUNT / PTHREAD_COUNT;
        pthreadInfo[i].to = pthreadInfo[i].from + ENEMIES_COUNT / PTHREAD_COUNT;
    }
#endif
    double deltaTime = 0;
    int bulletIndex = 0;

#define STAR_COUNT 1000
    Star stars[STAR_COUNT] = {0};

    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].x = (random() % (graphics.imageData.size.x));
        stars[i].y = (random() % (graphics.imageData.size.y));
        stars[i].z = (double)random() / (double)RAND_MAX;
    }

    float speed = -160.f;
    bool shouldExit = false;

#ifdef PTHREAD_COUNT
    pthread_t updateEnemyID[PTHREAD_COUNT];
    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthread_create(&updateEnemyID[i], NULL, updateEnemies, &pthreadInfo[i]);
    }
#endif

    while (!(shouldExit || this.shouldQuit))
    {
        deltaTime = getDeltaTime();
        graphicsUpdateMouseCoordinates(&graphics);

        if (isKeyJustPressed(graphics.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            this.shouldQuit = true;
        }

        if (isKeyJustPressed(graphics.window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        {
            shouldExit = true;
        }
        mainThreadBussy = true;
        graphicsClear(this.graphics.imageData);

        for (int i = 0; i < STAR_COUNT; i++)
        {
            stars[i].x = stars[i].x + speed * deltaTime * stars[i].z;
            stars[i].x = fmod((fmod(stars[i].x, graphics.imageData.size.x) +
                               graphics.imageData.size.x),
                              graphics.imageData.size.x);
            graphicsPutPixel(graphics.imageData, (PointI){stars[i].x, stars[i].y},
                             (Color){stars[i].z * 255, stars[i].z * 255, stars[i].z * 255});
        }
#ifndef PTHREAD_COUNT
        for (int i = 0; i < enemiesArray->header.length; i++)
        {
            Enemy *enemy = (Enemy *)arrayGetElementAt(enemiesArray, i);
            *enemy = enemyFlyingEggUpdateState(*enemy, deltaTime);
            enemy->sprite.position = enemy->movementDef.position;
            spriteDrawTransparentClipped(enemy->sprite, graphics);
        }
#endif

        this.hero.position = updatePositionBasedOnKeyboard(graphics.window,
                                                           this.hero.position, deltaTime, 100.0);
        spriteDrawTransparentClipped(this.hero, graphics.imageData);

        this.thrusters.position.y = this.hero.position.y + 6;
        this.thrusters.position.x = this.hero.position.x - 14;

        spriteDrawTransparentAnimatedClipped(&this.thrusters, graphics.imageData, deltaTime);

        if (isKeyJustPressed(graphics.window, GLFW_KEY_SPACE))
        {
            soundPlaySfx(this.sound, SFX_SHOOT_HERO);
            bulletIndex++;
            bulletIndex %= MAX_HERO_BULLETS_ON_SCREEN;
            this.heroBulletsPositions[bulletIndex].y = this.hero.position.y + this.hero.size.y * .5;
            this.heroBulletsPositions[bulletIndex].x = this.hero.position.x + this.projectiles[PROJECTILE_HERO].size.x * 0.5;
        }

        updateHeroProjectiles(&this, deltaTime);

        drawHeroProjectiles(this, graphics);

        printFPS(graphics, deltaTime);
        graphicsSwapBuffers(graphics);
        glfwPollEvents();
        mainThreadBussy = false;
    }

#ifdef PTHREAD_COUNT
    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthreadInfo[i].shouldQuit = true;
        pthread_join(updateEnemyID[i], NULL);
    }
    pthread_mutex_destroy(&lock);
#endif
    return this;
}
