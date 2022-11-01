// #include <stdio.h>
// #include <unistd.h>
// #define UR_SCREEN_WIDTH 33
// #define UR_SCREEN_HEIGHT 24
// #define UR_PUT_PIXEL urPutPixel

// #include "universal_renderer.h"

// char buffer[UR_SCREEN_WIDTH * UR_SCREEN_HEIGHT] = {0};

// void urPutPixel(URPointI point, URColor color)
// {
//     if (color.r == 255 && color.g == 0 && color.b == 255)
//     {
//         buffer[UR_SCREEN_WIDTH * point.y + point.x] = ' ';
//         return;
//     }

//     int targetColorI = (color.r + color.g + color.b) / 3;

//     char targetColorC = 33 + (targetColorI % (255 - 33));

//     buffer[UR_SCREEN_WIDTH * point.y + point.x] = targetColorC;
// }

// void drawScreen()
// {
//     for (int y = 0; y < UR_SCREEN_HEIGHT; y++)
//     {
//         printf("\n");
//     }
//     for (int y = 0; y < UR_SCREEN_HEIGHT; y++)
//     {
//         for (int x = 0; x < UR_SCREEN_WIDTH; x++)
//         {
//             printf("%c", buffer[UR_SCREEN_WIDTH * y + x]);
//         }
//         printf("\n");
//     }
//     usleep(250000);
// }

// int main(void)
// {
//     URSprite s = urSpriteCreate("assets/level2/heroGreen.bmp");

//     s.position.x = 0;
//     s.position.y = 0;
//     s.animated = true;
//     s.animation.frameCount = 6;
//     s.animation.frameWidth = 24;
//     s.animation.frameRate = 15;

//     // urClearScreen((URColor){32, 32, 32});
//     for (;;)
//     {
//         s = urSpriteDrawTransparentAnimatedClipped(s, 0.5);
//         drawScreen();
//     }

//     return 0;
// }