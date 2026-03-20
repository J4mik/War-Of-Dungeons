#pragma once

#include <fstream>
#include <iostream>

#include <cmath>
#include <cstdint>

#include "chunkgen.hpp"


#define TILESIZE 32
#define CHUNKSIZE 16
#define CHUNKSIZEPX (TILESIZE * CHUNKSIZE)
#define HALFTILESIZE (TILESIZE * 0.5)

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
                tilesTemp[tileX][tileY] = calculateTile(startX + tileX, startY + tileY);
                m_biome[tileX][tileY] = generateBiome(startX + tileX, startY + tileY);
            }
        }


        for (std::int32_t tileX = 0; tileX < CHUNKSIZE; ++tileX)
        {
            for (std::int32_t tileY = 0; tileY < CHUNKSIZE; ++tileY)
            {
                // m_tiles[tileX][tileY] = tilesTemp[tileX][tileY];
                m_tiles[tileX][tileY] = m_biome[tileX][tileY];
                m_tilegrid[tileX][tileY] = tilesTemp[tileX][tileY] * 8 + tilesTemp[tileX + 1][tileY] * 4 +
                    tilesTemp[tileX][tileY + 1] * 2 + tilesTemp[tileX + 1][tileY + 1];

                // if ( // m_tilegrid[tileX][tileY] != 0 && m_tilegrid[tileX][tileY] != 15 &&
                //     !(tilesTemp[tileX][tileY] == tilesTemp[tileX][tileY + 1] &&
                //       tilesTemp[tileX][tileY] == tilesTemp[tileX + 1][tileY] &&
                //       tilesTemp[tileX][tileY] == tilesTemp[tileX + 1][tileY + 1]))
                // {
                //     m_mask.emplace_back(position{tileX, tileY});
                // }


                // if (tilesTemp[tileX][tileY] || tilesTemp[tileX + 1][tileY] || tilesTemp[tileX][tileY + 1] ||
                //     tilesTemp[tileX + 1][tileY + 1])
                // {
                //     if (m_tilegrid[tileX][tileY] !=
                //         8 + (m_biome[tileX][tileY] == m_biome[tileX + 1][tileY]) * 4 +
                //             (m_biome[tileX][tileY] == m_biome[tileX][tileY + 1]) * 2 +
                //             (m_biome[tileX][tileY] == m_biome[tileX + 1][tileY + 1]))
                //     {
                //         m_tilegrid[tileX][tileY] = 0;
                //     }
                //     m_tiles[tileX][tileY] = m_biome[tileX - 1][tileY];
                //     m_underlay.emplace_back(tile{tileX, tileY, m_biome[tileX + 1][tileY]});
                //     // m_tilegrid[tileX][tileY] = 15;
                // }
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
