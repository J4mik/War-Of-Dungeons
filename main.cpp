#pragma once

#include <thread>
#include <vector>
#include "src/audio.hpp"
#include "src/render.hpp"

#define PLAYERSPEED 0.34

int spawnX = 0;
int spawnY = 0;

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO || SDL_INIT_AUDIO);
    TTF_Init();
    MIX_Init();

    SDL_Window* win = SDL_CreateWindow("War of Dungeons", screen.w, screen.h, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_Renderer* rend = SDL_CreateRenderer(win, NULL);
    SDL_SetRenderDrawColor(rend, 2, 15, 35, 255);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);


    SDL_Texture* playButton = IMG_LoadTexture(rend, "data/images/play.png");
    SDL_SetTextureScaleMode(playButton, SDL_SCALEMODE_NEAREST);

    SDL_FRect playButtonPos;
    Text ByteBounce("data/fonts/ByteBounce.ttf", 80);

    // while (!(mouseX > ((screen.w - 96) / 2) && mouseX < ((screen.w + 96) / 2) && mouseY > (screen.h / 2) && mouseY <
    // (screen.h / 2 + 36)) && running) {
    inputs();
    SDL_GetWindowSizeInPixels(win, &screen.w, &screen.h);
    SDL_RenderClear(rend);

    SDL_RenderTexture(rend, playButton, NULL, &playButtonPos);

    SDL_RenderPresent(rend);
    // SDL_Delay(10);
    // }

    // SDL_Texture* texture = IMG_LoadTexture(rend, "data/images/blocks.png");
    // SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    decay.init("./data/num.bin");

    srand(SEED);

    // calculates player spawn so I'ts always on land
    while (calculateHeight(spawnX, spawnY) < 0)
    {
        spawnX += rand() % 64 - 32;
        spawnY += rand() % 64 - 32;
        std::cout << spawnX << ", " << spawnY << "\n";
    }

    player.x = spawnX * TILESIZE;
    player.y = spawnY * TILESIZE;

    screen.posX = player.x;
    screen.posY = player.y;

    std::thread t1(loadChunks);

    inputs();

    std::thread t2(renderingLoop, win, rend);


    while (running)
    {
        inputs();

        player.VectX *= decay.pow255[deltaTime];
        player.VectY *= decay.pow255[deltaTime];

        // player rendering
        player.VectX += ((key.d || key.rightArrow) - (key.a || key.leftArrow)) * deltaTime *
            (1 - decay.pow255[deltaTime]) * PLAYERSPEED;
        player.VectY += ((key.s || key.downArrow) - (key.w || key.upArrow)) * deltaTime *
            (1 - decay.pow255[deltaTime]) * PLAYERSPEED;


        // clamps the player speed
        if (abs((key.d || key.rightArrow) - (key.a || key.leftArrow)) +
                abs((key.s || key.downArrow) - (key.w || key.upArrow)) >
            1)
        {
            player.x += player.VectX * 0.72;
            player.y += player.VectY * 0.72;
        }
        else
        {
            player.x += player.VectX;
            player.y += player.VectY;
        }

        playerPos.x = player.x + screen.ofsetX - screen.posX;
        playerPos.y = player.y + screen.ofsetY - screen.posY;


        SDL_Delay(1);
    }
    musicRunning = 0;
    running = 0;
    t1.join();
    t2.join();

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    TTF_Quit();
    MIX_Quit();
    return 0;
}
