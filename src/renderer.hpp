#ifndef renderer_h
#define renderer_h

#include "level.hpp"

typedef struct spriteInstance
{
	float x, y, z;
	float rotation;
	float w, h, padding_a, padding_b;
	float tex_u, tex_v, tex_w, tex_h;
	float r, g, b, a;
} spriteInstance;

void spriteRender(spriteInstance spriteData);

struct playerPos
{
    int x;
    int y;
    int w;
    int h;
}playerPos;

void spriteRenderCall()
{
    // static float ofsetX, ofsetY = screen.tempOfsetX / screen.w, screen.tempOfsetY / screen.h;
    screen.posX -= (screen.posX - player.x) * 0.15 * deltaTime * (1 - decay.pow255[deltaTime]);
    screen.posY -= (screen.posY - player.y) * 0.15 * deltaTime * (1 - decay.pow255[deltaTime]);
    screen.ofsetX = screen.w * 0.5;
    screen.ofsetY = screen.h * 0.5;
    screen.tempOfsetX = std::floor(screen.ofsetX - screen.posX);
    screen.tempOfsetY = std::floor(screen.ofsetY - screen.posY);
    playerPos.x = player.x + screen.ofsetX - screen.posX;
    playerPos.y = player.y + screen.ofsetY - screen.posY;

    for (unsigned int i = 0; i < chunks.size(); ++i)
    {
        for (unsigned int x = 0; x < CHUNKSIZE; ++x)
        {
            for (unsigned int y = 0; y < CHUNKSIZE; ++y)
            if (chunks[i].m_tilegrid[x][y] > 0)
            {
                spriteRender(
                    spriteInstance{
                    (float)(std::floor(chunks[i].x * CHUNKSIZEPX + x * TILESIZE) + screen.posX + HALFTILESIZE), 
                    (float)(std::floor(chunks[i].y * CHUNKSIZEPX + y * TILESIZE) + screen.posY + HALFTILESIZE), 0, 
                    0, 
                    32, 32, 0, 0, 
                    tilegridpos[chunks[i].m_tilegrid[x][y]].x, tilegridpos[chunks[i].m_tilegrid[x][y]].y, 0.25f, 0.25f, 
                    1.0f, 1.0f, 1.0f, 1.0f
                    });
            }
        }
    }
}

#endif