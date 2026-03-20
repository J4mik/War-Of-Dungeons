#pragma once

#include <thread>
#include <vector>
#include "../include/JSON/json.hpp"
#include "audio.hpp"
#include "engine.hpp"


#define GENERATECHUNKOFSCREENOFSET 200

#define PLAYERSPEED 0.34

using namespace nlohmann;

std::vector<chunk> chunks{};

sprite player;

int16_t minX;
int16_t minY;
int16_t maxX;
int16_t maxY;

int spawnX = 0;
int spawnY = 0;

bool tempFlag = 1;

void loadChunks()
{
    while (running)
    {

        // works out the minimum and maximum coordinates of chunks needed to fill the screen with a bit of headroom
        minX = std::floor((screen.posX - screen.w * 0.5 - GENERATECHUNKOFSCREENOFSET) / CHUNKSIZEPX);
        minY = std::floor((screen.posY - screen.h * 0.5 - GENERATECHUNKOFSCREENOFSET) / CHUNKSIZEPX);
        maxX = std::ceil((screen.posX + screen.w * 0.5 + GENERATECHUNKOFSCREENOFSET) / CHUNKSIZEPX);
        maxY = std::ceil((screen.posY + screen.h * 0.5 + GENERATECHUNKOFSCREENOFSET) / CHUNKSIZEPX);

        // std::cout << "(" << minX << ", " << minY << "), (" << maxX << ", " << maxY <<  ")\n";

        // checks which tiles to unload
        for (int i = 0; i < chunks.size(); ++i)
        {
            if (chunks[i].x < minX || chunks[i].x > maxX || chunks[i].y < minY || chunks[i].y > maxY)
            {
                std::swap(chunks[i], chunks.back());
                chunks.pop_back();
                --i;
            }
        }

        // checks which tiles to load
        for (int16_t x = minX; x < maxX + 1; ++x)
        {
            for (int16_t y = minY; y < maxY + 1; ++y)
            {
                for (int i = 0; i < chunks.size(); ++i)
                {
                    if (x == chunks[i].x && y == chunks[i].y)
                    {
                        tempFlag = 0;
                        // break;
                    }
                }
                if (tempFlag)
                {
                    chunks.emplace_back(chunk{});
                    chunks[chunks.size() - 1].loadChunk(x, y);
                    // if (!running) {
                    break;
                    // }
                }
                tempFlag = 1;
            }
        }
        // SDL_Delay(1);
    }
}

void playerMovement() {}

bool game(int lvl, SDL_Window* win, SDL_Renderer* rend)
{
    if (!running)
    {
        return 0;
    }

    SDL_FRect clip{0, 0, 16, 16};
    SDL_FRect temp{0, 0, TILESIZE, TILESIZE};
    SDL_FRect playerPos{0, 0, 32, 42};

    // SDL_Texture* texture = IMG_LoadTexture(rend, "data/images/blocks.png");
    // SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_Texture* tempTexture;
    SDL_Texture* tempTextureMask;

    SDL_Texture* grass = IMG_LoadTexture(rend, "data/images/GrassTiles.png");
    SDL_SetTextureScaleMode(grass, SDL_SCALEMODE_NEAREST);

    SDL_Texture* livelyGrass = IMG_LoadTexture(rend, "data/images/LivelyGrassTiles.png");
    SDL_SetTextureScaleMode(livelyGrass, SDL_SCALEMODE_NEAREST);

    SDL_Texture* darkGrass = IMG_LoadTexture(rend, "data/images/DarkGrassTiles.png");
    SDL_SetTextureScaleMode(darkGrass, SDL_SCALEMODE_NEAREST);

    SDL_Texture* ice = IMG_LoadTexture(rend, "data/images/IceTiles.png");
    SDL_SetTextureScaleMode(ice, SDL_SCALEMODE_NEAREST);

    SDL_Texture* dirt = IMG_LoadTexture(rend, "data/images/DirtTiles.png");
    SDL_SetTextureScaleMode(dirt, SDL_SCALEMODE_NEAREST);

    SDL_Texture* snow = IMG_LoadTexture(rend, "data/images/SnowTiles.png");
    SDL_SetTextureScaleMode(snow, SDL_SCALEMODE_NEAREST);

    SDL_Texture* sand = IMG_LoadTexture(rend, "data/images/SandTiles.png");
    SDL_SetTextureScaleMode(sand, SDL_SCALEMODE_NEAREST);

    SDL_Texture* redSand = IMG_LoadTexture(rend, "data/images/RedSandTiles.png");
    SDL_SetTextureScaleMode(redSand, SDL_SCALEMODE_NEAREST);

    SDL_Texture* stone = IMG_LoadTexture(rend, "data/images/StoneTiles.png");
    SDL_SetTextureScaleMode(stone, SDL_SCALEMODE_NEAREST);

    SDL_Texture* playerTexture = IMG_LoadTexture(rend, "data/images/player.png");
    SDL_SetTextureScaleMode(playerTexture, SDL_SCALEMODE_NEAREST);

    srand(SEED);

    // calculates player spawn so I'ts always on land
    while (!calculateTile(spawnX, spawnY))
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

    expDecay decay;
    decay.init("./data/num.bin");

    inputs();

    while (running)
    {
        inputs();

        SDL_GetWindowSizeInPixels(win, &screen.w, &screen.h);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 30, 100, 255, 255);


        // tile rendering
        screen.posX -= (screen.posX - player.x) * 0.15 * deltaTime * (1 - decay.pow255[deltaTime]);
        screen.posY -= (screen.posY - player.y) * 0.15 * deltaTime * (1 - decay.pow255[deltaTime]);
        screen.ofsetX = screen.w * 0.5;
        screen.ofsetY = screen.h * 0.5;
        screen.tempOfsetX = std::floor(screen.ofsetX - screen.posX);
        screen.tempOfsetY = std::floor(screen.ofsetY - screen.posY);

        // checks if chunks are on the screen
        for (int i = 0; i < chunks.size(); ++i)
        {

            // for (uint8_t n = 0; n < chunks[i].m_underlay.size(); ++n)
            // {
            //     temp.x = (chunks[i].m_underlay[n].x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX +
            //         HALFTILESIZE;
            //     temp.y = (chunks[i].m_underlay[n].y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY +
            //         HALFTILESIZE;
            //     clip.x = tilegridpos[chunks[i].m_tilegrid[chunks[i].m_underlay[n].x][chunks[i].m_underlay[n].y]].x;
            //     clip.y = tilegridpos[chunks[i].m_tilegrid[chunks[i].m_underlay[n].x][chunks[i].m_underlay[n].y]].y;
            //     clip.x = 32;
            //     clip.y = 16;
            //     SDL_RenderTexture(rend, grass, &clip, &temp);
            // }
            // loops through every tile in a chunk
            for (int x = 0; x < CHUNKSIZE; ++x)
            {
                for (int y = 0; y < CHUNKSIZE; ++y)
                {
                    if (chunks[i].m_biome[x][y] == 0)
                    {
                        tempTexture = dirt;
                    }
                    else if (chunks[i].m_biome[x][y] == 1)
                    {
                        tempTexture = sand;
                    }
                    else if (chunks[i].m_biome[x][y] == 2)
                    {
                        tempTexture = livelyGrass;
                    }
                    else if (chunks[i].m_biome[x][y] == 3)
                    {
                        tempTexture = stone;
                    }
                    if (chunks[i].m_biome[x][y] == 4)
                    {
                        tempTexture = grass;
                    }
                    else if (chunks[i].m_biome[x][y] == 5)
                    {
                        tempTexture = dirt;
                    }
                    else if (chunks[i].m_biome[x][y] == 6)
                    {
                        tempTexture = redSand;
                    }
                    else if (chunks[i].m_biome[x][y] == 7)
                    {
                        tempTexture = snow;
                    }
                    else if (chunks[i].m_biome[x][y] == 8)
                    {
                        tempTexture = ice;
                    }
                    else if (chunks[i].m_biome[x][y] == 9)
                    {
                        tempTexture = darkGrass;
                    }

                    if (chunks[i].m_tilegrid[x][y] != 0)
                    {
                        // tempTexture = darkGrass;

                        // renders chunk
                        temp.x = (x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX + HALFTILESIZE;
                        temp.y = (y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY + HALFTILESIZE;
                        clip.x = tilegridpos[chunks[i].m_tilegrid[x][y]].x;
                        clip.y = tilegridpos[chunks[i].m_tilegrid[x][y]].y;
                        SDL_RenderTexture(rend, tempTexture, &clip, &temp);
                    }
                }
            }
        }

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

        SDL_RenderTexture(rend, playerTexture, NULL, &playerPos);

        SDL_RenderPresent(rend);

        SDL_Delay(3);
    }
    musicRunning = 0;
    running = 0;
    t1.join();
    return 0;
}
