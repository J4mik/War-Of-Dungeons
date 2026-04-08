#include <cmath>
// #include <stdint.h>

#include "engine.hpp"
#include "level.hpp"
#include "chunkgen.hpp"

std::basic_string<char> levelPath;

void chunk::generateChunk()
{
    int32_t startX = CHUNKSIZE * x;
    int32_t startY = CHUNKSIZE * y;

    for (int tileX = 0; tileX < CHUNKSIZE + 1; ++tileX)
    {
        for (int tileY = 0; tileY < CHUNKSIZE + 1; ++tileY)
        {
            m_biome[tileX][tileY] = generateBiome(startX + tileX, startY + tileY, &tilesTemp[tileX][tileY]);
        }
    }


    for (int tileX = 0; tileX < CHUNKSIZE + 1; ++tileX)
    {
        for (int tileY = 0; tileY < CHUNKSIZE; ++tileY)
        {
            m_tiles[tileX][tileY] = m_biome[tileX][tileY];
        }
    }


    for (int tileX = 0; tileX < CHUNKSIZE; ++tileX)
    {
        for (int tileY = 0; tileY < CHUNKSIZE; ++tileY)
        {
            m_tilegrid[tileX][tileY] = tilesTemp[tileX][tileY] * 8 | tilesTemp[tileX + 1][tileY] * 4 |
                tilesTemp[tileX][tileY + 1] * 2 | tilesTemp[tileX + 1][tileY + 1];


             m_overlay[tileX][tileY] =
                (char((m_biome[tileX][tileY + 1] == m_biome[tileX + 1][tileY]) &&
                     (m_biome[tileX][tileY + 1] != m_biome[tileX + 1][tileY + 1]) && tilesTemp[tileX + 1][tileY] &&
                      tilesTemp[tileX][tileY + 1] && tilesTemp[tileX + 1][tileY + 1]) << 3) |
                (char((m_biome[tileX + 1][tileY + 1] == m_biome[tileX][tileY]) &&
                      (m_biome[tileX][tileY] != m_biome[tileX][tileY + 1]) && tilesTemp[tileX][tileY] &&
                      tilesTemp[tileX][tileY + 1] && tilesTemp[tileX + 1][tileY + 1]) << 2) |
                (char((m_biome[tileX][tileY] == m_biome[tileX + 1][tileY + 1]) &&
                      (m_biome[tileX][tileY] != m_biome[tileX + 1][tileY]) && tilesTemp[tileX + 1][tileY] &&
                      tilesTemp[tileX][tileY] && tilesTemp[tileX + 1][tileY + 1]) << 1) |
                (char(m_biome[tileX][tileY + 1] == m_biome[tileX + 1][tileY]) &&
                      (m_biome[tileX][tileY + 1] != m_biome[tileX][tileY]) && tilesTemp[tileX + 1][tileY] &&
                      tilesTemp[tileX][tileY + 1] && tilesTemp[tileX][tileY]);
        }
    }
    m_stored = false;
}

void chunk::loadChunk(int16_t posX, int16_t posY)
{
    m_stored = true;

    x = posX;
    y = posY;

    generateChunk();
}

void loadChunks()
{
    bool tempFlag = 1;

    int16_t minX;
    int16_t minY;
    int16_t maxX;
    int16_t maxY;
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
                    break;
                }
                else
                {
                    tempFlag = 1;
                }
            }
        }
        // SDL_Delay(1);
    }
}