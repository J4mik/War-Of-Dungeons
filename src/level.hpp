#pragma once

#include <fstream>
#include <iostream>

#include <cmath>
#include <cstdint>

#include "chunkgen.hpp"


#define TILESIZE 4
#define CHUNKSIZE 16
#define CHUNKSIZEPX (TILESIZE * CHUNKSIZE)

class position
{
public:
    int16_t x;
    int16_t y;
};

class chunk
{
public:
    int16_t x; // multiplied by 8 to save unecessary bytes that would be wasted by storing as a multiple of 8
    int16_t y;
    bool tiles[CHUNKSIZE][CHUNKSIZE] = {}; // array of tiles
    int biome[CHUNKSIZE][CHUNKSIZE] = {};
    void generateChunk()
    {
        int32_t startX = CHUNKSIZE * x;
        int32_t startY = CHUNKSIZE * y;


        for (std::int32_t tileX = 0; tileX < CHUNKSIZE; ++tileX)
        {
            for (std::int32_t tileY = 0; tileY < CHUNKSIZE; ++tileY)
            {
                tiles[tileX][tileY] = calculateTile(startX + tileX, startY + tileY);
                biome[tileX][tileY] = generateBiome(startX + tileX, startY + tileY);
            }
        }
        m_stored = false;
    }
    void storeChunk()
    {
        if (!m_stored)
        {
            m_stored = true;
        }
    }
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
