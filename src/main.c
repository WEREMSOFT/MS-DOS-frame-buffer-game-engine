#include <stdio.h>

#define UR_SCREEN_WIDTH 150
#define UR_SCREEN_HEIGHT 80
#define UR_PUT_PIXEL urPutPixel

#include "universal_renderer.h"

char buffer[UR_SCREEN_WIDTH * UR_SCREEN_HEIGHT] = {0};

void urPutPixel(URPointI point, Color color)
{
    int targetColorI = (color.r + color.g + color.b) / 3;

    char targetColorC = 33 + (targetColorI % (255 - 33));

    buffer[UR_SCREEN_WIDTH * point.y + point.x] = targetColorC;
}

void drawScreen()
{
    for (int y = 0; y < UR_SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < UR_SCREEN_WIDTH; x++)
        {
            printf("%c", buffer[UR_SCREEN_WIDTH * y + x]);
        }
        printf("-\n");
    }
}

int main(void)
{
    URContext context = {0};
    URSprite s = urSpriteCreate("assets/enemyGreenBig1.bmp");
    context.width = 80;
    context.height = 25;
    urClearScreen(context, (Color){32, 32, 32});
    urDrawCircle((URPointI){40, 11}, 5.0, (Color){'X', 'X', 'X'});
    // urDrawLine((URPointI){0, 0}, (URPointI){10, 10}, (Color){'A', 'A', 'A'});
    urPrintString((URPointI){1, 1}, "hello", (Color){'O', 'O', 'O'});
    urSpriteDrawClipped(s);
    drawScreen();
    return 0;
}