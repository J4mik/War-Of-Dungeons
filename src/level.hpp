#pragma once

#include <fstream>
#include <iostream>

#include <cmath>
#include <cstdint>

#include "../include/perlin/perlin.hpp"

// int16_t tiles[0][2];

#define FREQUENCY1 0.02 // should be between 0.1 and 64
#define MULTIPLIER1 0.75
#define FREQUENCY2 0.065 // should be between 0.1 and 64
#define MULTIPLIER2 0.2
#define FREQUENCY3 0.2 // should be between 0.1 and 64
#define MULTIPLIER3 0.05

#define OCTAVES 3 // has to be between 1 and 16
#define THRESHOLD 0.5
#define SEED 0


#define TILESIZE 36
#define CHUNKSIZE 32
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
    int tiles[CHUNKSIZE][CHUNKSIZE] = {}; // array of tiles
    void generateChunk()
    {
        int32_t startX = CHUNKSIZE * x;
        int32_t startY = CHUNKSIZE * y;

        std::uint32_t seed = SEED;
        std::int32_t octaves = OCTAVES;

        const siv::PerlinNoise perlin{seed};

        for (std::int32_t tileY = 0; tileY < CHUNKSIZE; ++tileY)
        {
            for (std::int32_t tileX = 0; tileX < CHUNKSIZE; ++tileX)
            {
                if ((perlin.octave2D_01(((startX + tileX) * FREQUENCY1), ((startY + tileY) * FREQUENCY1), octaves) *
                         MULTIPLIER1 +
                     perlin.octave2D_01(((startX + tileX) * FREQUENCY2), ((startY + tileY) * FREQUENCY2), octaves) *
                         MULTIPLIER2 +
                     perlin.octave2D_01(((startX + tileX) * FREQUENCY3), ((startY + tileY) * FREQUENCY3), octaves) *
                         MULTIPLIER3) >= THRESHOLD)
                {
                    tiles[tileX][tileY] = 1;
                }
            }
        }
        m_stored = false;
    }
    void storeChunk() {
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
