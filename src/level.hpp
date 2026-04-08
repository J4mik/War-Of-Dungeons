#ifndef level_h
#define level_h

#include <vector>
#include "engine.hpp"

#define TILESIZE 32
#define CHUNKSIZE 16
#define CHUNKSIZEPX (TILESIZE * CHUNKSIZE)
#define HALFTILESIZE (TILESIZE * 0.5)

#define GENERATECHUNKOFSCREENOFSET 100

class tile
{
public:
    int16_t x;
    int16_t y;
    uint16_t tile;
};

struct position
{
    int x;
    int y;
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
    // std::vector<tile> m_underlay = {};
    // std::vector<position> m_mask = {};
    void generateChunk();

    void loadChunk(int16_t posX, int16_t posY);

private:
    bool m_stored = false;
};

std::vector<chunk> chunks{};

void loadChunks();

#endif