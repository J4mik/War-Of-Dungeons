#include <thread>
#include <vector>
#include "src/audio.hpp"
#include "src/render.hpp"

#define PLAYERSPEED 0.36

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

    // Text ByteBounce("data/fonts/ByteBounce.ttf", 80);/

    inputs();
    SDL_GetWindowSizeInPixels(win, &screen.w, &screen.h);
    SDL_RenderClear(rend);

    SDL_RenderPresent(rend);

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

    inputs();

    std::thread t1(loadChunks);

    std::thread t2(renderingLoop, win, rend);


    while (running)
    {
        inputs();

        screen.posX -= (screen.posX - player.x) * deltaTime * (1 - decay.pow255[deltaTime]);
        screen.posY -= (screen.posY - player.y) * deltaTime * (1 - decay.pow255[deltaTime]);

        player.VectX *= decay.pow255[deltaTime * 3];
        player.VectY *= decay.pow255[deltaTime * 3];

        // player rendering
        player.VectX += ((key.d || key.rightArrow) - (key.a || key.leftArrow)) *
            (1 - decay.pow255[deltaTime * 3]) * PLAYERSPEED;
        player.VectY += ((key.s || key.downArrow) - (key.w || key.upArrow)) *
            (1 - decay.pow255[deltaTime * 3]) * PLAYERSPEED;


        // clamps the player speed
        if (abs((key.d || key.rightArrow) - (key.a || key.leftArrow)) +
                abs((key.s || key.downArrow) - (key.w || key.upArrow)) >
            1)
        {
            player.x += player.VectX * 0.72 * deltaTime;
            player.y += player.VectY * 0.72 * deltaTime;
        }
        else
        {
            player.x += player.VectX * deltaTime;
            player.y += player.VectY * deltaTime;
        }

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
