#include "render.hpp"

void loadTiles(SDL_Renderer* rend)
{
    grass = IMG_LoadTexture(rend, "data/images/GrassTiles.png");
    SDL_SetTextureScaleMode(grass, SDL_SCALEMODE_NEAREST);

    livelyGrass = IMG_LoadTexture(rend, "data/images/LivelyGrassTiles.png");
    SDL_SetTextureScaleMode(livelyGrass, SDL_SCALEMODE_NEAREST);

    darkGrass = IMG_LoadTexture(rend, "data/images/DarkGrassTiles.png");
    SDL_SetTextureScaleMode(darkGrass, SDL_SCALEMODE_NEAREST);

    driedGrass = IMG_LoadTexture(rend, "data/images/DriedGrassTiles.png");
    SDL_SetTextureScaleMode(driedGrass, SDL_SCALEMODE_NEAREST);

    ice = IMG_LoadTexture(rend, "data/images/IceTiles.png");
    SDL_SetTextureScaleMode(ice, SDL_SCALEMODE_NEAREST);

    dirt = IMG_LoadTexture(rend, "data/images/DirtTiles.png");
    SDL_SetTextureScaleMode(dirt, SDL_SCALEMODE_NEAREST);

    snow = IMG_LoadTexture(rend, "data/images/SnowTiles.png");
    SDL_SetTextureScaleMode(snow, SDL_SCALEMODE_NEAREST);

    sand = IMG_LoadTexture(rend, "data/images/SandTiles.png");
    SDL_SetTextureScaleMode(sand, SDL_SCALEMODE_NEAREST);

    redSand = IMG_LoadTexture(rend, "data/images/RedSandTiles.png");
    SDL_SetTextureScaleMode(redSand, SDL_SCALEMODE_NEAREST);

    stone = IMG_LoadTexture(rend, "data/images/StoneTiles.png");
    SDL_SetTextureScaleMode(stone, SDL_SCALEMODE_NEAREST);

    playerTexture = IMG_LoadTexture(rend, "data/images/player.png");
    SDL_SetTextureScaleMode(playerTexture, SDL_SCALEMODE_NEAREST);
}

void getTile(uint8_t tile)
{
    switch (tile)
    {
    case 0:
        tempTexture = driedGrass;
        break;
    case 1:
        tempTexture = sand;
        break;
    case 2:
        tempTexture = livelyGrass;
        break;
    case 3:
        tempTexture = stone;
        break;
    case 4:
        tempTexture = grass;
        break;
    case 5:
        tempTexture = dirt;
        break;
    case 6:
        tempTexture = redSand;
        break;
    case 7:
        tempTexture = snow;
        break;
    case 8:
        tempTexture = ice;
        break;
    case 9:
        tempTexture = darkGrass;
        break;

    default:
        // tempTexture = dirt;
        break;
    }
}


void renderingLoop(SDL_Window* win) {
    expDecay decay;

    SDL_FRect playerPos{0, 0, 32, 42};

    SDL_FRect tempOverlay{0, 0, 16, 16};
    SDL_FRect clipOverlay{0, 0, 8, 8};

    SDL_FRect clip{0, 0, 16, 16};
    SDL_FRect temp{0, 0, TILESIZE, TILESIZE};

    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV || SDL_GPU_SHADERFORMAT_MSL || SDL_GPU_SHADERFORMAT_DXIL, false, nullptr);

    SDL_ClaimWindowForGPUDevice(device, win);

    SDL_Renderer* rend = SDL_CreateGPURenderer(win, 0, &device);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);


    loadTiles(rend);

    // tile rendering
    while (running)
    {  

        SDL_FlushRenderer(rend);

        screen.ofsetX = screen.w * 0.5;
        screen.ofsetY = screen.h * 0.5;
        screen.tempOfsetX = std::floor(screen.ofsetX - screen.posX);
        screen.tempOfsetY = std::floor(screen.ofsetY - screen.posY);
        playerPos.x = player.x + screen.ofsetX - screen.posX;
        playerPos.y = player.y + screen.ofsetY - screen.posY;

        
        SDL_GetWindowSizeInPixels(win, &screen.w, &screen.h);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 30, 100, 255, 255);

        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);

        // get the swapchain texture
        SDL_GPUTexture* swapchainTexture;
        Uint32 width, height;
        // SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, win, &swapchainTexture, &width, &height);

        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.clear_color = {240/255.0f, 240/255.0f, 240/255.0f, 255/255.0f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        // colorTargetInfo.texture = swapchainTexture;

        // begin a render pass
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

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
                    if ((chunks[i].m_overlay[x][y] & 2) == 2)
                    {
                        tempOverlay.x =
                            (x * TILESIZE + chunks[i].x * CHUNKSIZEPX) + screen.tempOfsetX + HALFTILESIZE * 3;
                        tempOverlay.y =
                            (y * TILESIZE + chunks[i].y * CHUNKSIZEPX) + screen.tempOfsetY + HALFTILESIZE * 2;
                        clipOverlay.x = 0;
                        clipOverlay.y = 56;
                        SDL_RenderTexture(rend, tempTexture, &clipOverlay, &tempOverlay);
                    }

                    if ((chunks[i].m_overlay[x][y] & 8) < 2)
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
                    if (chunks[i].m_overlay[x][y] & 1)
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

        // draw something
        SDL_RenderPresent(rend);

        // // end the render pass
        // SDL_EndGPURenderPass(renderPass);

        // // submit the command buffer
        // SDL_SubmitGPUCommandBuffer(commandBuffer);


    }
}