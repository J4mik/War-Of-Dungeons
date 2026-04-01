#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_shadercross.h>

#include <iostream>

static SDL_GPUGraphicsPipeline* RenderPipeline;
static SDL_GPUSampler* Sampler;
static SDL_GPUTexture* Texture;
static SDL_GPUTransferBuffer* SpriteDataTransferBuffer; // sprite batching transfer buffer
static SDL_GPUBuffer* SpriteDataBuffer; // sprite batching buffer
static const Uint32 SPRITE_COUNT = 512;
static const char* BasePath = NULL;

typedef struct SpriteInstance
{
    float x, y, z;
    float rotation;
    float w, h, padding_a, padding_b;
    float tex_u, tex_v, tex_w, tex_h;
    float r, g, b, a;
} SpriteInstance;

typedef struct Matrix4x4
{
    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;
} Matrix4x4;

Matrix4x4 Matrix4x4_CreateOrthographicOffCenter(
    float left,
    float right,
    float bottom,
    float top,
    float zNearPlane,
    float zFarPlane
) {
    return (Matrix4x4) {
        2.0f / (right - left), 0, 0, 0,
        0, 2.0f / (top - bottom), 0, 0,
        0, 0, 1.0f / (zNearPlane - zFarPlane), 0,
        (left + right) / (left - right), (top + bottom) / (bottom - top), zNearPlane / (zNearPlane - zFarPlane), 1
    };
}

SDL_GPUShader* LoadShader(
    SDL_GPUDevice* device,
    const char* shaderFilename,
    Uint32 samplerCount,
    Uint32 uniformBufferCount,
    Uint32 storageBufferCount,
    Uint32 storageTextureCount
) {
    // Auto-detect the shader stage from the file name for convenience
    SDL_GPUShaderStage stage;
    if (SDL_strstr(shaderFilename, ".vert"))
    {
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
    }
    else if (SDL_strstr(shaderFilename, ".frag"))
    {
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    }
    else
    {
        SDL_Log("Invalid shader stage!");
        return NULL;
    }

    char fullPath[256];
    SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
    const char *entrypoint;

    if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.spv", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.msl", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_MSL;
        entrypoint = "main0";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.dxil", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entrypoint = "main";
    } else {
        SDL_Log("%s", "Unrecognized backend shader format!");
        return NULL;
    }

    size_t codeSize;
    void* code = SDL_LoadFile(fullPath, &codeSize);
    if (code == NULL)
    {
        SDL_Log("Failed to load shader from disk! %s", fullPath);
        return NULL;
    }

    SDL_GPUShaderCreateInfo shaderInfo = {
        .code_size = codeSize,
        .code =  (Uint8*)code,
        .entrypoint = entrypoint,
        .format = format,
        .stage = stage,
        .num_samplers = samplerCount,
        .num_storage_textures = storageTextureCount,
        .num_storage_buffers = storageBufferCount,
        .num_uniform_buffers = uniformBufferCount
    };
    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
    if (shader == NULL)
    {
        SDL_Log("Failed to create shader!");
        SDL_free(code);
        return NULL;
    }

    SDL_free(code);
    return shader;
}

SDL_Surface* LoadImage(const char* imageFilename, int desiredChannels)
{
    char fullPath[256];
    SDL_Surface *result;
    SDL_PixelFormat format;

    SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s", BasePath, imageFilename);

    result = SDL_LoadBMP(fullPath);
    if (result == NULL)
    {
        SDL_Log("Failed to load BMP: %s", SDL_GetError());
        return NULL;
    }

    if (desiredChannels == 4)
    {
        format = SDL_PIXELFORMAT_ABGR8888;
    }
    else
    {
        SDL_assert(!"Unexpected desiredChannels");
        SDL_DestroySurface(result);
        return NULL;
    }
    if (result->format != format)
    {
        SDL_Surface *next = SDL_ConvertSurface(result, format);
        SDL_DestroySurface(result);
        result = next;
    }

    return result;
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
        
    BasePath = SDL_GetBasePath();

    // Create SDL Window
    SDL_Window* window = SDL_CreateWindow("SDL3 Unicode Text", 420, 300, SDL_WINDOW_VULKAN  | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        return -1;
    }

    SDL_GPUDevice* device = SDL_CreateGPUDevice(
                                                SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
                                                false,
                                                "claimDevice");

    if (!device)
    {
        printf("SDL_CreateGPUDevice failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window))
    {
        SDL_Log("GPUClaimWindow failed");
        return -1;
    }

    SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
    if (SDL_WindowSupportsGPUPresentMode(
        device,
        window,
        SDL_GPU_PRESENTMODE_IMMEDIATE
    )) {
        presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
    }
    else if (SDL_WindowSupportsGPUPresentMode(
        device,
        window,
        SDL_GPU_PRESENTMODE_MAILBOX
    )) {
        presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
    }

    SDL_SetGPUSwapchainParameters(
        device,
        window,
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        presentMode
    );

    // Create the shaders
    SDL_GPUShader* vertShader = LoadShader(
        device,
        "shaders/vertex.spv",
        0,
        1,
        1,
        0
    );

    SDL_GPUShader* fragShader = LoadShader(
        device,
        "shaders/fragment.spv",
        1,
        0,
        0,
        0
    );


    SDL_GPUColorTargetDescription target_desc[] {{
        .format = SDL_GetGPUSwapchainTextureFormat(device, window),
        .blend_state = {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .enable_blend = true
        }
    }};
        
    SDL_GPUGraphicsPipelineTargetInfo pipeline_target_info{
        .color_target_descriptions = target_desc,
        .num_color_targets = 1
    };

    SDL_GPUGraphicsPipelineCreateInfo pipeline_info{
        .vertex_shader = vertShader,
        .fragment_shader = fragShader,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = pipeline_target_info
    };

    // Create the sprite render pipeline
    RenderPipeline = SDL_CreateGPUGraphicsPipeline(
        device,
        &pipeline_info
    );

    SDL_ReleaseGPUShader(device, vertShader);
    SDL_ReleaseGPUShader(device, fragShader);

    // Load the image data
    SDL_Surface *imageData = LoadImage("data/images/player.png", 4);
    if (imageData == NULL)
    {
        SDL_Log("Could not load image data!");
        return -1;
    }

    SDL_GPUTransferBufferCreateInfo buffer_info {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<Uint32>(imageData->w * imageData->h * 4)
    };

    SDL_GPUTransferBuffer* textureTransferBuffer = SDL_CreateGPUTransferBuffer(
        device,
        &buffer_info
    );

    Uint8 *textureTransferPtr = (Uint8*) SDL_MapGPUTransferBuffer(
        device,
        textureTransferBuffer,
        false
    );
    SDL_memcpy(textureTransferPtr, imageData->pixels, imageData->w * imageData->h * 4);
    SDL_UnmapGPUTransferBuffer(device, textureTransferBuffer);

    SDL_GPUTextureCreateInfo texture_info{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = static_cast<Uint32>(imageData->w),
        .height = static_cast<Uint32>(imageData->h),
        .layer_count_or_depth = 1,
        .num_levels = 1
    };

    // Create the GPU resources
    Texture = SDL_CreateGPUTexture(
        device,
        &texture_info
    );

    SDL_GPUSamplerCreateInfo sampler_info {
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE
    };

    Sampler = SDL_CreateGPUSampler(
        device,
        &sampler_info
    );

    SDL_GPUTransferBufferCreateInfo transfer_buffer_info {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = SPRITE_COUNT * sizeof(SpriteInstance)
    };

    SpriteDataTransferBuffer = SDL_CreateGPUTransferBuffer(
        device,
        &transfer_buffer_info
    );

    SDL_GPUBufferCreateInfo buffer_info_read {
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = SPRITE_COUNT * sizeof(SpriteInstance)
    };

    SpriteDataBuffer = SDL_CreateGPUBuffer(
        device,
        &buffer_info_read
    );

    // Transfer the up-front data
    SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

    SDL_GPUTextureTransferInfo transfer_info {
        .transfer_buffer = textureTransferBuffer,
        .offset = 0, /* Zeroes out the rest */
    };

    SDL_GPUTextureRegion texture_region {
        .texture = Texture,
        .w = static_cast<Uint32>(imageData->w),
        .h = static_cast<Uint32>(imageData->h),
        .d = 1
    };

    SDL_UploadToGPUTexture(
        copyPass,
        &transfer_info,
        &texture_region,
        false
    );

    SDL_GPUTextureSamplerBinding sampler_binding {
        .texture = Texture,
        .sampler = Sampler
    };

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);

    SDL_DestroySurface(imageData);
    SDL_ReleaseGPUTransferBuffer(device, textureTransferBuffer);

    bool running = true;
    SDL_Event event;

    static float uCoords[4] = { 0.0f, 0.5f, 0.0f, 0.5f };
    static float vCoords[4] = { 0.0f, 0.0f, 0.5f, 0.5f };

    while (running)
    {
        // Handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        Matrix4x4 cameraMatrix = Matrix4x4_CreateOrthographicOffCenter(
                0,
                640,
                480,
                0,
                0,
                -1
            );

            SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer(device);
            if (cmdBuf == NULL)
            {
                SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
                return -1;
            }

            SDL_GPUTexture* swapchainTexture;
            if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuf, window, &swapchainTexture, NULL, NULL)) {
                SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
                return -1;
            }

            if (swapchainTexture != NULL)
            {
                // Build sprite instance transfer
                SpriteInstance* dataPtr = (SpriteInstance*)SDL_MapGPUTransferBuffer(
                    device,
                    SpriteDataTransferBuffer,
                    true
                );

                int j = 0;
                for (Uint32 i = 0; i < SPRITE_COUNT; i += 1)
                {
                    dataPtr[i].x = (32*i)%640;
                    dataPtr[i].y = (64*j)%480;
                    dataPtr[i].z = 0;
                    dataPtr[i].rotation = 0;
                    dataPtr[i].w = 24;
                    dataPtr[i].h = 24;
                    dataPtr[i].tex_u = 0;
                    dataPtr[i].tex_v = 0;
                    dataPtr[i].tex_w = 1.0f;
                    dataPtr[i].tex_h = 1.0f;
                    dataPtr[i].r = 1.0f;
                    dataPtr[i].g = 1.0f;
                    dataPtr[i].b = 1.0f;
                    dataPtr[i].a = 1.0f;
                    
                    if(i%20 == 0)
                        j++;
                }

                SDL_UnmapGPUTransferBuffer(device, SpriteDataTransferBuffer);

                SDL_GPUTransferBufferLocation buffer_location {
                    .transfer_buffer = SpriteDataTransferBuffer,
                    .offset = 0
                };
                
                SDL_GPUBufferRegion buffer_region {
                    .buffer = SpriteDataBuffer,
                    .offset = 0,
                    .size = SPRITE_COUNT * sizeof(SpriteInstance)
                };
                
                // Upload instance data
                SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuf);
                SDL_UploadToGPUBuffer(
                    copyPass,
                    &buffer_location,
                    &buffer_region,
                    true
                );
                SDL_EndGPUCopyPass(copyPass);

                SDL_GPUColorTargetInfo target_info{
                    .texture = swapchainTexture,
                    .clear_color = { 0, 0, 0, 1 },
                    .load_op = SDL_GPU_LOADOP_CLEAR,
                    .store_op = SDL_GPU_STOREOP_STORE,
                    .cycle = false
                };
                
                // Render sprites
                SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
                    cmdBuf,
                    &target_info,
                    1,
                    NULL
                );

                SDL_BindGPUGraphicsPipeline(renderPass, RenderPipeline);
                SDL_BindGPUVertexStorageBuffers(
                    renderPass,
                    0,
                    &SpriteDataBuffer,
                    1
                );
                SDL_GPUTextureSamplerBinding sampler_binding{
                    .texture = Texture,
                    .sampler = Sampler
                };
                SDL_BindGPUFragmentSamplers(
                    renderPass,
                    0,
                    &sampler_binding,
                    1
                );
                SDL_PushGPUVertexUniformData(
                    cmdBuf,
                    0,
                    &cameraMatrix,
                    sizeof(Matrix4x4)
                );
                SDL_DrawGPUPrimitives(
                    renderPass,
                    SPRITE_COUNT * 6,
                    1,
                    0,
                    0
                );

                SDL_EndGPURenderPass(renderPass);
            }

            SDL_SubmitGPUCommandBuffer(cmdBuf);


        // Present the frame
        SDL_GL_SwapWindow(window);
    }
    return 0;
}