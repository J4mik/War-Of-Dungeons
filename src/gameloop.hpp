#pragma once

#include "../include/JSON/json.hpp"
#include "audio.hpp"
#include "engine.hpp"


using namespace nlohmann;

chunk mein;

#define TILESIZE 12

bool game(int lvl, SDL_Window* win, SDL_Renderer* rend)
{
    if (!running)
    {
        return 0;
    }

    SDL_FRect clip{0, 0, 12, 12};
    SDL_FRect temp{0, 0, TILESIZE, TILESIZE};

    SDL_Texture* texture = IMG_LoadTexture(rend, "data/images/blocks.png");
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    inputs();

    while (running)
    {
        inputs();

        SDL_GetWindowSizeInPixels(win, &screen.w, &screen.h);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 2, 7, 19, 255);

        temp = {0, 0, TILESIZE, TILESIZE};

        SDL_RenderTexture(rend, texture, &clip, &temp);


        for (int x = 0; x < CHUNKSIZE; ++x)
        {
            for (int y = 0; y < CHUNKSIZE; ++y)
            {
                if (mein.tiles[x][y] == 1)
                {
                    temp.x = (x * TILESIZE);
                    temp.y = (y * TILESIZE);
                    SDL_RenderTexture(rend, texture, &clip, &temp);
                }
            }
        }

        SDL_RenderPresent(rend);

        SDL_Delay(3);
    }
    musicRunning = 0;
    return 0;
}
