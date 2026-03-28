
#include "level.hpp"
#include "loadTiles.hpp"

expDecay decay;

SDL_FRect playerPos{0, 0, 32, 42};

// the vertex input layout
struct Vertex
{
    float x, y, z;      //vec3 position
    float r, g, b, a;   //vec4 color
};

// a list of vertices
static Vertex vertices[]
{
    {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},     // top vertex
    {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},   // bottom left vertex
    {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}     // bottom right vertex
};

void renderingLoop(SDL_Window* win) {
    SDL_FRect tempOverlay{0, 0, 16, 16};
    SDL_FRect clipOverlay{0, 0, 8, 8};

    SDL_FRect clip{0, 0, 16, 16};
    SDL_FRect temp{0, 0, TILESIZE, TILESIZE};

    SDL_Renderer* rend = SDL_CreateRenderer(win, NULL);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);


    loadTiles(rend);

    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV || SDL_GPU_SHADERFORMAT_MSL || SDL_GPU_SHADERFORMAT_DXIL, false, nullptr);

    SDL_ClaimWindowForGPUDevice(device, win);

    // tile rendering
    while (running)
    {  
        screen.ofsetX = screen.w * 0.5;
        screen.ofsetY = screen.h * 0.5;
        screen.tempOfsetX = std::floor(screen.ofsetX - screen.posX);
        screen.tempOfsetY = std::floor(screen.ofsetY - screen.posY);
        playerPos.x = player.x + screen.ofsetX - screen.posX;
        playerPos.y = player.y + screen.ofsetY - screen.posY;

        
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


        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);

        // get the swapchain texture
        SDL_GPUTexture* swapchainTexture;
        Uint32 width, height;
        SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, win, &swapchainTexture, &width, &height);

        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.clear_color = {240/255.0f, 240/255.0f, 240/255.0f, 255/255.0f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.texture = swapchainTexture;

        // begin a render pass
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

        // draw something

        // end the render pass
        SDL_EndGPURenderPass(renderPass);

        // submit the command buffer
        SDL_SubmitGPUCommandBuffer(commandBuffer);


        SDL_RenderPresent(rend);
    }
}