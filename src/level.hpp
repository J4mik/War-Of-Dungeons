#pragma once

#include <fstream>
#include <iostream>

#include <cmath>
#include <cstdint>

#include "../include/perlin/perlin.hpp"

// int16_t tiles[0][2];

#define FREQUENCY1 0.015 // should be between 0.1 and 64
#define MULTIPLIER1 0.75
#define FREQUENCY2 0.05 // should be between 0.1 and 64
#define MULTIPLIER2 0.2
#define FREQUENCY3 0.2 // should be between 0.1 and 64
#define MULTIPLIER3 0.05
#define OCTAVES 3 // has to be between 1 and 16
#define SEED 0
#define CHUNKSIZE 256
#define THRESHOLD 0.54

class tile
{
public:
    int16_t x;
    int16_t y;
    int16_t type;
};

class chunk
{
public:
    int16_t x; // multiplied by 8 to save unecessary bytes that would be wasted by storing as a multiple of 8
    int16_t y;
    int tiles[CHUNKSIZE][CHUNKSIZE] = {}; // array of tiles
    void generateChunk(int16_t posX, int16_t posY)
    {

        int32_t startX = CHUNKSIZE * posX;
        int32_t startY = CHUNKSIZE * posY;

        std::uint32_t seed = SEED;
        std::int32_t octaves = OCTAVES;

        const siv::PerlinNoise perlin{seed};

        for (std::int32_t y = 0; y < CHUNKSIZE; ++y)
        {
            for (std::int32_t x = 0; x < CHUNKSIZE; ++x)
            {
                if ((perlin.octave2D_01(((startX + x) * FREQUENCY1), ((startY + y) * FREQUENCY1), octaves) *
                         MULTIPLIER1 +
                     perlin.octave2D_01(((startX + x) * FREQUENCY2), ((startY + y) * FREQUENCY2), octaves) *
                         MULTIPLIER2 +
                     perlin.octave2D_01(((startX + x) * FREQUENCY3), ((startY + y) * FREQUENCY3), octaves) *
                         MULTIPLIER3) >= THRESHOLD)
                {
                    tiles[x][y] = 1;
                }
            }
        }
        m_stored = false;
    }
    void storeChunk() { m_stored = true; }

private:
    bool m_stored = false;
};
