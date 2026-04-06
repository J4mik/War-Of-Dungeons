#include <fstream>
#include <iostream>

#include <cmath>
#include <cstdint>

#include "level.hpp"

std::basic_string<char> levelPath;

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