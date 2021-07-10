#include <pthread.h>
#include "level.h"
#include "../assetManager/assetManager.h"
#include "levelStaticFunctions.h"
#include "../utils/utils.h"
#include "../core/input/keyboard.h"
#include "../core/stackAllocator/staticAlloc.h"
#include <stdlib.h>
#include <soloud_c.h>
#include <sys/time.h>
#include <unistd.h>

#define PTHREAD_COUNT 1
#define ENEMIES_COUNT 50
#define STAR_COUNT 1000

Level levelCreate(Graphics graphics, Sprite *sprites, Sound sound)
{
    Level this = {0};
    this.graphics = graphics;
    this.enemyLayer.imageData = graphics.imageData;
    this.sound = sound;
    this.hero = sprites[ASSET_SHIP_BLUE];
    this.projectiles[PROJECTILE_HERO] = sprites[ASSET_HERO_BULLET];
    this.thrusters = sprites[ASSET_SHIP_THRUSTERS];
    this.enemy = sprites[ASSET_ENEMY_SHOOTING_EGG];
    this.shouldQuit = false;
    return this;
}

void *updateEnemies(void *params)
{

    PthreadInfo *this = (PthreadInfo *)params;

    int imageDataIndex = 0;
    double lastUpdate = 0;
    double deltaTime = 0;
    Array *enemiesArray = this->enemiesArray;
    struct timeval start, stop;
    ImageData imageDatas[2] = {0};

    imageDatas[0] = this->graphics.imageData;
    imageDatas[1] = this->graphics.imageData;
    imageDatas[0].data = (Color *)allocStatic(this->graphics.imageData.bufferSize);
    imageDatas[1].data = (Color *)allocStatic(this->graphics.imageData.bufferSize);

    this->enemyLayer->imageData = imageDatas[imageDataIndex];

    while (!this->shouldQuit)
    {
        gettimeofday(&start, NULL);
        imageDataIndex++;
        imageDataIndex %= 2;
        double actualTime = glfwGetTime();
        deltaTime = actualTime - lastUpdate;
        lastUpdate = actualTime;
        imClearTransparent(imageDatas[imageDataIndex]);
        for (int i = this->from; i < this->to; i++)
        {
            Enemy *enemy = (Enemy *)arrayGetElementAt(enemiesArray, i);
            *enemy = enemyFlyingEggUpdateState(*enemy, deltaTime);
            enemy->sprite.position = enemy->movementDef.position;
            spriteDrawTransparentClipped(enemy->sprite, imageDatas[imageDataIndex]);
        }
        pthread_mutex_lock(this->lock);
        this->enemyLayer->imageData = imageDatas[imageDataIndex];
        pthread_mutex_unlock(this->lock);
        gettimeofday(&stop, NULL);
        unsigned long int elapsedUSec = (stop.tv_sec - start.tv_sec) / 1000000 + stop.tv_usec - start.tv_usec;
        if (elapsedUSec < 1.6666)
        {
            usleep(1.6666 - elapsedUSec);
        }
    }
    return NULL;
}

Level levelMainLoop(Level this)
{
    Graphics graphics = this.graphics;
    double deltaTime = 0;
    int bulletIndex = 0;
    PthreadInfo pthreadInfo[PTHREAD_COUNT] = {0};
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

    pthread_mutex_init(&this.lock, NULL);

    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthreadInfo[i].graphics = this.graphics;
        pthreadInfo[i].enemyLayer = &this.enemyLayer;
        pthreadInfo[i].screen = graphics.imageData;
        pthreadInfo[i].enemiesArray = enemiesArray;
        pthreadInfo[i].from = i * ENEMIES_COUNT / PTHREAD_COUNT;
        pthreadInfo[i].to = pthreadInfo[i].from + ENEMIES_COUNT / PTHREAD_COUNT;
        pthreadInfo[i].lock = &this.lock;
    }

    Star stars[STAR_COUNT] = {0};

    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].x = (random() % (graphics.imageData.size.x));
        stars[i].y = (random() % (graphics.imageData.size.y));
        stars[i].z = (double)random() / (double)RAND_MAX;
    }

    float speed = -160.f;
    bool shouldExit = false;

    pthread_t updateEnemyID[PTHREAD_COUNT];
    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthread_create(&updateEnemyID[i], NULL, updateEnemies, &pthreadInfo[i]);
    }

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
        imClear(this.graphics.imageData);

        for (int i = 0; i < STAR_COUNT; i++)
        {
            stars[i].x = stars[i].x + speed * deltaTime * stars[i].z;
            stars[i].x = fmod((fmod(stars[i].x, graphics.imageData.size.x) +
                               graphics.imageData.size.x),
                              graphics.imageData.size.x);
            imPutPixel(graphics.imageData, (PointI){stars[i].x, stars[i].y},
                       (Color){stars[i].z * 255, stars[i].z * 255, stars[i].z * 255});
        }
        pthread_mutex_lock(&this.lock);
        spriteDrawTransparent(this.enemyLayer, graphics.imageData);
        pthread_mutex_unlock(&this.lock);

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
            this.heroBulletsPositions[bulletIndex].y = this.hero.position.y + this.hero.imageData.size.y * .5;
            this.heroBulletsPositions[bulletIndex].x = this.hero.position.x + this.projectiles[PROJECTILE_HERO].imageData.size.x * 0.5;
        }

        updateHeroProjectiles(&this, deltaTime);

        drawHeroProjectiles(this, graphics);

        printFPS(graphics, deltaTime);
        graphicsSwapBuffers(graphics);
        glfwPollEvents();
    }

    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthreadInfo[i].shouldQuit = true;
        pthread_join(updateEnemyID[i], NULL);
    }
    return this;
}

void levelDestroy(Level this)
{
    pthread_mutex_destroy(&this.lock);
}
