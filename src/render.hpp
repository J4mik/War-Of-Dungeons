
#include "level.hpp"
#include "loadTiles.hpp"

expDecay decay;

SDL_FRect playerPos{0, 0, 32, 42};

void renderingLoop(SDL_Window* win, SDL_Renderer* rend) {
    SDL_FRect tempOverlay{0, 0, 16, 16};
    SDL_FRect clipOverlay{0, 0, 8, 8};

    SDL_FRect clip{0, 0, 16, 16};
    SDL_FRect temp{0, 0, TILESIZE, TILESIZE};
    loadTiles(rend);

    // tile rendering
    while (running)
    {  
        screen.posX -= (screen.posX - player.x) * 0.15 * deltaTime * (1 - decay.pow255[deltaTime]);
        screen.posY -= (screen.posY - player.y) * 0.15 * deltaTime * (1 - decay.pow255[deltaTime]);
        screen.ofsetX = screen.w * 0.5;
        screen.ofsetY = screen.h * 0.5;
        screen.tempOfsetX = std::floor(screen.ofsetX - screen.posX);
        screen.tempOfsetY = std::floor(screen.ofsetY - screen.posY);

        
        SDL_GetWindowSizeInPixels(win, &screen.w, &screen.h);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 30, 100, 255, 255);

        // checks if chunks are on the screen
        for (int i = 0; i < chunks.size(); ++i)
        {
            for (int x = 0; x < CHUNKSIZE; ++x)
            {
                for (int y = 0; y < CHUNKSIZE; ++y)
                {
                    getTile(chunks[i].m_tiles[x][y]);

                    if (chunks[i].m_tilegrid[x][y] != 0)
                    {
                        // renders chunk
                        temp.x = (x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX + HALFTILESIZE;
                        temp.y = (y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY + HALFTILESIZE;
                        clip.x = tilegridpos[chunks[i].m_tilegrid[x][y]].x;
                        clip.y = tilegridpos[chunks[i].m_tilegrid[x][y]].y;
                        SDL_RenderTexture(rend, tempTexture, &clip, &temp);
                    }
                }
            }
        }

        for (int i = 0; i < chunks.size(); ++i)
        {
            for (int x = 0; x < CHUNKSIZE; ++x)
            {
                for (int y = 0; y < CHUNKSIZE; ++y)
                {
                    getTile(chunks[i].m_tiles[x][y]);

                    if ((chunks[i].m_overlay[x][y] & 4) == 4)
                    {
                        tempOverlay.x =
                            (x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX + HALFTILESIZE * 2;
                        tempOverlay.y =
                            (y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY + HALFTILESIZE * 3;
                        clipOverlay.x = 8;
                        clipOverlay.y = 48;
                        SDL_RenderTexture(rend, tempTexture, &clipOverlay, &tempOverlay);
                    }
                    else if ((chunks[i].m_overlay[x][y] & 2) == 2)
                    {
                        tempOverlay.x =
                            (x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX + HALFTILESIZE * 3;
                        tempOverlay.y =
                            (y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY + HALFTILESIZE * 2;
                        clipOverlay.x = 0;
                        clipOverlay.y = 56;
                        SDL_RenderTexture(rend, tempTexture, &clipOverlay, &tempOverlay);
                    }
                    else
                    {
                    getTile(chunks[i].m_tiles[x + 1][y]);
                    }

                    if ((chunks[i].m_overlay[x][y] & 8) == 8)
                    {
                        tempOverlay.x =
                            (x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX + HALFTILESIZE * 3;
                        tempOverlay.y =
                            (y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY + HALFTILESIZE * 3;
                        clipOverlay.x = 0;
                        clipOverlay.y = 48;
                        SDL_RenderTexture(rend, tempTexture, &clipOverlay, &tempOverlay);
                    }
                    else if (chunks[i].m_overlay[x][y] & 1)
                    {
                        tempOverlay.x =
                            (x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX + HALFTILESIZE * 2;
                        tempOverlay.y =
                            (y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY + HALFTILESIZE * 2;
                        clipOverlay.x = 8;
                        clipOverlay.y = 56;
                        SDL_RenderTexture(rend, tempTexture, &clipOverlay, &tempOverlay);
                    }
                }
            }
        }
        SDL_RenderTexture(rend, playerTexture, NULL, &playerPos);
        SDL_RenderPresent(rend);

    }
}