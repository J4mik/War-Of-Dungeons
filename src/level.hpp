#include <fstream>
#include <iostream>

#include <cmath>
#include <cstdint>

#include "chunkgen.hpp"
#include "engine.hpp"

std::basic_string<char> levelPath;

#define TILESIZE 32
#define CHUNKSIZE 16
#define CHUNKSIZEPX (TILESIZE * CHUNKSIZE)
#define HALFTILESIZE (TILESIZE * 0.5)

#define GENERATECHUNKOFSCREENOFSET 100

class position
{
public:
    int16_t x;
    int16_t y;
};

class tile
{
public:
    int16_t x;
    int16_t y;
    uint16_t tile;
};

position tilegridpos[16] = {{0, 48},  {16, 48}, {0, 0},   {48, 0}, {0, 32},  {16, 0},  {32, 48}, {16, 16},
                            {48, 48}, {0, 16},  {48, 32}, {32, 0}, {16, 32}, {32, 32}, {48, 16}, {32, 16}};

bool tilesTemp[CHUNKSIZE + 1][CHUNKSIZE + 1] = {};

class chunk
{
public:
    int16_t x; // multiplied by 8 to save unecessary bytes that would be wasted by storing as a multiple of 8
    int16_t y;
    uint16_t m_tiles[CHUNKSIZE][CHUNKSIZE] = {}; // array of tiles
    char m_tilegrid[CHUNKSIZE][CHUNKSIZE] = {};
    uint16_t m_biome[CHUNKSIZE + 1][CHUNKSIZE + 1] = {};
    char m_overlay[CHUNKSIZE][CHUNKSIZE] = {};
    // std::vector<tile> m_underlay = {};
    // std::vector<position> m_mask = {};
    void generateChunk()
    {
        int32_t startX = CHUNKSIZE * x;
        int32_t startY = CHUNKSIZE * y;

        for (std::int32_t tileX = 0; tileX < CHUNKSIZE + 1; ++tileX)
        {
            for (std::int32_t tileY = 0; tileY < CHUNKSIZE + 1; ++tileY)
            {
                m_biome[tileX][tileY] = generateBiome(startX + tileX, startY + tileY, &tilesTemp[tileX][tileY]);
            }
        }


        for (std::int32_t tileX = 0; tileX < CHUNKSIZE + 1; ++tileX)
        {
            for (std::int32_t tileY = 0; tileY < CHUNKSIZE; ++tileY)
            {
                m_tiles[tileX][tileY] = m_biome[tileX][tileY];
            }
        }


        for (std::int32_t tileX = 0; tileX < CHUNKSIZE; ++tileX)
        {
            for (std::int32_t tileY = 0; tileY < CHUNKSIZE; ++tileY)
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
    // void storeChunk()
    // {
    //     if (!m_stored)
    //     {
    //         std::ifstream FileStream;
    //         FileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    //         try
    //         {
    //             FileStream.open(levelPath, std::ios::in | std::ios::binary);
    //             for (int i = 0; i < 1024; ++i)
    //             {
    //                 FileStream.read((char*)&pow255[i], 8);
    //             }
    //             FileStream.close();
    //         }
    //         catch (std::ifstream::failure& error)
    //         {
    //             if (!pow255[0] == 1)
    //             {
    //                 std::ofstream FileStream;
    //                 FileStream.open(levelPath, std::ios::out | std::ios::binary);
    //                 pow255[0] = 1;
    //                 FileStream.write((char*)&pow255[0], 8);
    //                 for (int i = 1; i < 1024; ++i)
    //                 {
    //                     pow255[i] = pow255[i - 1] * 0.985;
    //                     FileStream.write((char*)&pow255[i], 8);
    //                 }
    //                 FileStream.close();
    //             }
    //         }
    //         m_stored = true;
    //     }
    // }
    void loadChunk(int16_t posX, int16_t posY)
    {
        m_stored = true;

        x = posX;
        y = posY;

        generateChunk();
    }

private:
    bool m_stored = false;
};

std::vector<chunk> chunks{};

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