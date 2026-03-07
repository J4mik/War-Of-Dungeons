#pragma once

#include "../include/JSON/json.hpp"
#include "audio.hpp"
#include "engine.hpp"
#include <vector>
#include <thread>

#define GENERATECHUNKOFSCREENOFSET 20

#define PLAYERSPEED 480

using namespace nlohmann;

std::vector<chunk> chunks{};

std::vector<position> unloadedChunks{};

sprite player;

int minX;
int minY;
int maxX;
int maxY;

bool game(int lvl, SDL_Window* win, SDL_Renderer* rend)
{
    if (!running)
    {
        return 0;
    }

    SDL_FRect clip{0, 0, 12, 12};
    SDL_FRect temp{0, 0, TILESIZE, TILESIZE};
    SDL_FRect playerPos{0, 0, 12, 20};

    SDL_Texture* texture = IMG_LoadTexture(rend, "data/images/blocks.png");
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_Texture* playerTexture = IMG_LoadTexture(rend, "data/images/player.png");
    SDL_SetTextureScaleMode(playerTexture, SDL_SCALEMODE_NEAREST);


    for (int x = -4; x < 4; ++x)
    {
        for (int y = -3; y < 3; ++y)
        {
            chunks.emplace_back(chunk{});
            chunks[chunks.size() - 1].loadChunk(x, y);
        }
    }


    inputs();

    while (running)
    {
        inputs();
        
        SDL_GetWindowSizeInPixels(win, &screen.w, &screen.h);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 30, 32, 33, 255);


        // tile rendering
        screen.posX -= (screen.posX - player.x) * 0.015 * deltaTime;
        screen.posY -= (screen.posY - player.y) * 0.015 * deltaTime;
        screen.ofsetX = screen.w * 0.5;
        screen.ofsetY = screen.h * 0.5;
        screen.tempOfsetX = std::floor(screen.ofsetX - screen.posX);
        screen.tempOfsetY = std::floor(screen.ofsetY - screen.posY);


        // works out the minimum and maximum coordinates of chunks needed to fill the screen with a bit of headroom
        minX = std::floor((screen.posX - screen.w * 0.5 - GENERATECHUNKOFSCREENOFSET) / CHUNKSIZEPX);
        minY = std::floor((screen.posY - screen.h * 0.5 - GENERATECHUNKOFSCREENOFSET) / CHUNKSIZEPX);
        maxX = std::ceil((screen.posX + screen.w * 0.5 + GENERATECHUNKOFSCREENOFSET) / CHUNKSIZEPX);
        maxY = std::ceil((screen.posY + screen.h * 0.5 + GENERATECHUNKOFSCREENOFSET) / CHUNKSIZEPX);
        
        for (int x = minX; x < maxX + 1; ++x)
        {
            for (int y = minY; y < maxY + 1; ++y)
            {
                unloadedChunks.emplace_back(position{x, y});
            }
        }


        // checks if chuks are on the screen
        for (int i = 0; i < chunks.size(); ++i) {
            if (colidetect(SDL_FRect{
                float(chunks[i].x * CHUNKSIZEPX + screen.tempOfsetX),
                float(chunks[i].y * CHUNKSIZEPX + screen.tempOfsetY),
                CHUNKSIZEPX, CHUNKSIZEPX},
                SDL_FRect{-GENERATECHUNKOFSCREENOFSET, -GENERATECHUNKOFSCREENOFSET, 
                float(screen.w + GENERATECHUNKOFSCREENOFSET), 
                float(screen.h + GENERATECHUNKOFSCREENOFSET)}))
            {

                // loops through every tile in a chunk
                for (int x = 0; x < CHUNKSIZE; ++x)
                {
                    for (int y = 0; y < CHUNKSIZE; ++y)
                    {
                        if (chunks[i].tiles[x][y] == 1)
                        {
                            // renders chunk
                            temp.x = (x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX;
                            temp.y = (y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY;
                            SDL_RenderTexture(rend, texture, &clip, &temp);
                        }
                    }
                }  
            }
            else
            {
                // deletes chunks that are of the screen
                // std::swap(chunks[i], chunks.back());
                // chunks.pop_back();
                // --i;
            }
            for (int o = 0; o < unloadedChunks.size(); ++o) 
            {
                if (unloadedChunks[o].x == chunks[i].x && unloadedChunks[o].y == chunks[i].y)
                {
                    std::swap(unloadedChunks[i], unloadedChunks.back());
                    unloadedChunks.pop_back();
                    // --o;
                    break;
                }
            }   
        }


        while (unloadedChunks.size() > 0) 
        {
            chunks.emplace_back(chunk{});
            chunks[chunks.size() - 1].loadChunk(unloadedChunks[0].x, unloadedChunks[0].y);
            std::swap(unloadedChunks[0], unloadedChunks.back());
            unloadedChunks.pop_back();
        }

        // player rendering
        player.VectX += ((key.d || key.rightArrow) - (key.a || key.leftArrow)) * deltaTime * PLAYERSPEED * 0.0001;
        player.VectY += ((key.s || key.downArrow) - (key.w || key.upArrow)) * deltaTime * PLAYERSPEED * 0.0001;

        player.VectX *= (50 - deltaTime) * 0.02;
        player.VectY *= (50 - deltaTime) * 0.02;


        // clamps the player speed
        if (abs((key.d || key.rightArrow) - (key.a || key.leftArrow)) + abs((key.s || key.downArrow) - (key.w || key.upArrow)) > 1) {
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

        SDL_RenderTexture(rend, playerTexture, NULL, &playerPos);

        SDL_RenderPresent(rend);

        SDL_Delay(3);
    }
    musicRunning = 0;
    return 0;
}
