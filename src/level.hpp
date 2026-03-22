#pragma once

#include <fstream>
#include <iostream>

#include <cmath>
#include <cstdint>

#include "chunkgen.hpp"


#define TILESIZE 32
#define CHUNKSIZE 16
#define CHUNKSIZEPX (TILESIZE * CHUNKSIZE)
#define HALFTILESIZE (TILESIZE >> 1)

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
    uint16_t m_tiles[CHUNKSIZE + 1][CHUNKSIZE] = {}; // array of tiles
    char m_tilegrid[CHUNKSIZE][CHUNKSIZE] = {};
    uint16_t m_biome[CHUNKSIZE + 1][CHUNKSIZE + 1] = {};
    char m_overlay[CHUNKSIZE][CHUNKSIZE] = {};
    void generateChunk()
    {
        int32_t startX = CHUNKSIZE * x;
        int32_t startY = CHUNKSIZE * y;

        for (std::int32_t tileX = 0; tileX < CHUNKSIZE + 1; ++tileX)
        {
            for (std::int32_t tileY = 0; tileY < CHUNKSIZE + 1; ++tileY)
            {
                tilesTemp[tileX][tileY] = calculateTile(startX + tileX, startY + tileY);
                m_biome[tileX][tileY] = generateBiome(startX + tileX, startY + tileY);
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
                        (char((m_biome[tileX][tileY + 1] == m_biome[tileX + 1][tileY]) && (m_biome[tileX][tileY + 1] != m_biome[tileX + 1][tileY + 1]) && tilesTemp[tileX + 1][tileY] && tilesTemp[tileX][tileY + 1] &&
                    tilesTemp[tileX + 1][tileY + 1]) << 3) |
                        (char((m_biome[tileX + 1][tileY + 1] == m_biome[tileX][tileY]) && (m_biome[tileX][tileY] != m_biome[tileX][tileY + 1]) && tilesTemp[tileX][tileY] && tilesTemp[tileX][tileY + 1] &&
                    tilesTemp[tileX + 1][tileY + 1]) << 2) |
                        (char((m_biome[tileX][tileY] == m_biome[tileX + 1][tileY + 1]) && (m_biome[tileX][tileY] != m_biome[tileX + 1][tileY]) && tilesTemp[tileX + 1][tileY] && tilesTemp[tileX][tileY] &&
                    tilesTemp[tileX + 1][tileY + 1]) << 1) |
                        (char(m_biome[tileX][tileY + 1] == m_biome[tileX + 1][tileY]) && (m_biome[tileX][tileY + 1] != m_biome[tileX][tileY]) && tilesTemp[tileX + 1][tileY] && tilesTemp[tileX][tileY + 1] &&
                    tilesTemp[tileX][tileY]);

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
