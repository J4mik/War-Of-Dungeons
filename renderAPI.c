#include "renderAPI.h"
#include "stb_image.h"
#include <SDL3/SDL_image.h>

unsigned int count = 0;

int w, h, quit;

keyPress key;

int CommonInit(Context* context, SDL_WindowFlags windowFlags, int windowX, int windowY)
{
	context->Device = SDL_CreateGPUDevice(
		SDL_GPU_SHADERFORMAT_SPIRV, // | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
		true,
		NULL);

	if (context->Device == NULL)
	{
		SDL_Log("GPUCreateDevice failed");
		return -1;
	}

	context->Window = SDL_CreateWindow("War Of Dungeons", windowX, windowY, windowFlags);
	if (context->Window == NULL)
	{
		SDL_Log("CreateWindow failed: %s", SDL_GetError());
		return -1;
	}

	if (!SDL_ClaimWindowForGPUDevice(context->Device, context->Window))
	{
		SDL_Log("GPUClaimWindow failed");
		return -1;
	}

	return 0;
}

void CommonQuit(Context* context)
{
	SDL_ReleaseWindowFromGPUDevice(context->Device, context->Window);
	SDL_DestroyWindow(context->Window);
	SDL_DestroyGPUDevice(context->Device);
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
		SDL_snprintf(fullPath, sizeof(fullPath), "Content/Shaders/Compiled/SPIRV/%s.spv", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "Content/Shaders/Compiled/MSL/%s.msl", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main0";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "Content/Shaders/Compiled/DXIL/%s.dxil", shaderFilename);
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
		.code = code,
		.code_size = codeSize,
		.entrypoint = entrypoint,
		.format = format,
		.stage = stage,
		.num_samplers = samplerCount,
		.num_uniform_buffers = uniformBufferCount,
		.num_storage_buffers = storageBufferCount,
		.num_storage_textures = storageTextureCount
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

SDL_GPUComputePipeline* CreateComputePipelineFromShader(
	SDL_GPUDevice* device,
	const char* shaderFilename,
	SDL_GPUComputePipelineCreateInfo *createInfo
) {
	char fullPath[256];
	SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	const char *entrypoint;

	if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
		SDL_snprintf(fullPath, sizeof(fullPath), "Content/Shaders/Compiled/SPIRV/%s.spv", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "Content/Shaders/Compiled/MSL/%s.msl", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "Content/Shaders/Compiled/DXIL/%s.dxil", shaderFilename);
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
		SDL_Log("Failed to load compute shader from disk! %s", fullPath);
		return NULL;
	}

	// Make a copy of the create data, then overwrite the parts we need
	SDL_GPUComputePipelineCreateInfo newCreateInfo = *createInfo;
	newCreateInfo.code = code;
	newCreateInfo.code_size = codeSize;
	newCreateInfo.entrypoint = entrypoint;
	newCreateInfo.format = format;

	SDL_GPUComputePipeline* pipeline = SDL_CreateGPUComputePipeline(device, &newCreateInfo);
	if (pipeline == NULL)
	{
		SDL_Log("Failed to create compute pipeline!");
		SDL_free(code);
		return NULL;
	}

	SDL_free(code);
	return pipeline;
}

SDL_Surface* LoadImage(const char* imageFilename, int desiredChannels)
{
	char fullPath[256];
	SDL_Surface *result;
	SDL_PixelFormat format;

	SDL_snprintf(fullPath, sizeof(fullPath), imageFilename);

	result = IMG_Load(fullPath);
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

// Matrix Math
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

ComputeSpriteInstance* dataPtr;

void renderSprite(ComputeSpriteInstance spriteRenderInstance)
{
	dataPtr[count] = spriteRenderInstance;
	++count;
}

int renderer()
{
	Context context = { 0 };
	float lastTime = 0;

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	bool canDraw = true;

	int result = CommonInit(&context, SDL_WINDOW_RESIZABLE, 900, 700);
	if (result < 0)
	{
		return result;
	}

	SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
	if (SDL_WindowSupportsGPUPresentMode(
		(&context)->Device,
		(&context)->Window,
		SDL_GPU_PRESENTMODE_IMMEDIATE
	)) {
		presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
	}
	else if (SDL_WindowSupportsGPUPresentMode(
		(&context)->Device,
		(&context)->Window,
		SDL_GPU_PRESENTMODE_MAILBOX
	)) {
		presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
	}

	SDL_SetGPUSwapchainParameters(
		(&context)->Device,
		(&context)->Window,
		SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
		presentMode
	);

	SDL_srand(0);

	// Create the shaders
	SDL_GPUShader* vertShader = LoadShader(
		(&context)->Device,
		"TexturedQuadColorWithMatrix.vert",
		0,
		1,
		0,
		0
	);

	SDL_GPUShader* fragShader = LoadShader(
		(&context)->Device,
		"TexturedQuadColor.frag",
		1,
		0,
		0,
		0
	);

	// Create the sprite render pipeline
	RenderPipeline = SDL_CreateGPUGraphicsPipeline(
		(&context)->Device,
		&(SDL_GPUGraphicsPipelineCreateInfo){
			.target_info = (SDL_GPUGraphicsPipelineTargetInfo){
				.num_color_targets = 1,
				.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
					.format = SDL_GetGPUSwapchainTextureFormat((&context)->Device, (&context)->Window),
					.blend_state = {
						.enable_blend = true,
						.color_blend_op = SDL_GPU_BLENDOP_ADD,
						.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
						.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
						.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
						.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
						.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
					}
				}}
			},
			.vertex_input_state = (SDL_GPUVertexInputState){
				.num_vertex_buffers = 1,
				.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
					.slot = 0,
					.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
					.instance_step_rate = 0,
					.pitch = sizeof(PositionTextureColorVertex)
				}},
				.num_vertex_attributes = 3,
				.vertex_attributes = (SDL_GPUVertexAttribute[]){{
					.buffer_slot = 0,
					.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
					.location = 0,
					.offset = 0
				}, {
					.buffer_slot = 0,
					.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
					.location = 1,
					.offset = 16
				}, {
					.buffer_slot = 0,
					.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
					.location = 2,
					.offset = 32
				}}
			},
			.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
			.vertex_shader = vertShader,
			.fragment_shader = fragShader
		}
	);

	SDL_ReleaseGPUShader((&context)->Device, vertShader);
	SDL_ReleaseGPUShader((&context)->Device, fragShader);

	// Create the sprite batch compute pipeline
	ComputePipeline = CreateComputePipelineFromShader(
		(&context)->Device,
		"SpriteBatch.comp",
		&(SDL_GPUComputePipelineCreateInfo){
			.num_readonly_storage_buffers = 1,
			.num_readwrite_storage_buffers = 1,
			.threadcount_x = 64,
			.threadcount_y = 1,
			.threadcount_z = 1
		}
	);

	// Load the image data
	SDL_Surface *imageData = LoadImage("data/images/GrassTiles.png", 4);
	if (imageData == NULL)
	{
		SDL_Log("Could not load image data!");
		return -1;
	}

	SDL_GPUTransferBuffer* textureTransferBuffer = SDL_CreateGPUTransferBuffer(
		(&context)->Device,
		&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = imageData->w * imageData->h * 4
		}
	);

	Uint8 *textureTransferPtr = SDL_MapGPUTransferBuffer(
		(&context)->Device,
		textureTransferBuffer,
		false
	);
	SDL_memcpy(textureTransferPtr, imageData->pixels, imageData->w * imageData->h * 4);
	SDL_UnmapGPUTransferBuffer((&context)->Device, textureTransferBuffer);

	// Create the GPU resources
	Texture = SDL_CreateGPUTexture(
		(&context)->Device,
		&(SDL_GPUTextureCreateInfo){
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.width = imageData->w,
			.height = imageData->h,
			.layer_count_or_depth = 1,
			.num_levels = 1,
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
		}
	);

	Sampler = SDL_CreateGPUSampler(
		(&context)->Device,
		&(SDL_GPUSamplerCreateInfo){
			.min_filter = SDL_GPU_FILTER_NEAREST,
			.mag_filter = SDL_GPU_FILTER_NEAREST,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE
		}
	);

	SpriteComputeTransferBuffer = SDL_CreateGPUTransferBuffer(
		(&context)->Device,
		&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = SPRITE_COUNT * sizeof(ComputeSpriteInstance)
		}
	);

	SpriteComputeBuffer = SDL_CreateGPUBuffer(
		(&context)->Device,
		&(SDL_GPUBufferCreateInfo) {
			.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ,
			.size = SPRITE_COUNT * sizeof(ComputeSpriteInstance)
		}
	);

	SpriteVertexBuffer = SDL_CreateGPUBuffer(
		(&context)->Device,
		&(SDL_GPUBufferCreateInfo) {
			.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_BUFFERUSAGE_VERTEX,
			.size = SPRITE_COUNT * 4 * sizeof(PositionTextureColorVertex)
		}
	);

	SpriteIndexBuffer = SDL_CreateGPUBuffer(
		(&context)->Device,
		&(SDL_GPUBufferCreateInfo) {
			.usage = SDL_GPU_BUFFERUSAGE_INDEX,
			.size = SPRITE_COUNT * 6 * sizeof(Uint32)
		}
	);

	// Transfer the up-front data
	SDL_GPUTransferBuffer* indexBufferTransferBuffer = SDL_CreateGPUTransferBuffer(
		(&context)->Device,
		&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = SPRITE_COUNT * 6 * sizeof(Uint32)
		}
	);

	Uint32* indexTransferPtr = SDL_MapGPUTransferBuffer(
		(&context)->Device,
		indexBufferTransferBuffer,
		false
	);

	for (Uint32 i = 0, j = 0; i < SPRITE_COUNT * 6; i += 6, j += 4)
	{
		indexTransferPtr[i]     =  j;
		indexTransferPtr[i + 1] =  j + 1;
		indexTransferPtr[i + 2] =  j + 2;
		indexTransferPtr[i + 3] =  j + 3;
		indexTransferPtr[i + 4] =  j + 2;
		indexTransferPtr[i + 5] =  j + 1;
	}

	SDL_UnmapGPUTransferBuffer(
		(&context)->Device,
		indexBufferTransferBuffer
	);

	SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer((&context)->Device);
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

	SDL_UploadToGPUTexture(
		copyPass,
		&(SDL_GPUTextureTransferInfo) {
			.transfer_buffer = textureTransferBuffer,
			.offset = 0, /* Zeroes out the rest */
		},
		&(SDL_GPUTextureRegion){
			.texture = Texture,
			.w = imageData->w,
			.h = imageData->h,
			.d = 1
		},
		false
	);

	SDL_UploadToGPUBuffer(
		copyPass,
		&(SDL_GPUTransferBufferLocation) {
			.transfer_buffer = indexBufferTransferBuffer,
			.offset = 0
		},
		&(SDL_GPUBufferRegion) {
			.buffer = SpriteIndexBuffer,
			.offset = 0,
			.size = SPRITE_COUNT * 6 * sizeof(Uint32)
		},
		false
	);

	SDL_DestroySurface(imageData);
	SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
	SDL_ReleaseGPUTransferBuffer((&context)->Device, textureTransferBuffer);
	SDL_ReleaseGPUTransferBuffer((&context)->Device, indexBufferTransferBuffer);

	while (!quit)
	{
		SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            quit = 1;
            break;
        case SDL_EVENT_KEY_DOWN:
            switch (event.key.scancode)
            {
            case SDL_SCANCODE_W:
                key.w = 1;
                break;
            case SDL_SCANCODE_A:
                key.a = 1;
                break;
            case SDL_SCANCODE_S:
                key.s = 1;
                break;
            case SDL_SCANCODE_D:
                key.d = 1;
                break;

            case SDL_SCANCODE_R:
                key.r = 1;
                break;

            case SDL_SCANCODE_UP:
                key.upArrow = 1;
                break;
            case SDL_SCANCODE_LEFT:
                key.leftArrow = 1;
                break;
            case SDL_SCANCODE_DOWN:
                key.downArrow = 1;
                break;
            case SDL_SCANCODE_RIGHT:
                key.rightArrow = 1;
                break;
            }
            break;

        case SDL_EVENT_KEY_UP:
            switch (event.key.scancode)
            {
            case SDL_SCANCODE_W:
                key.w = 0;
                break;
            case SDL_SCANCODE_A:
                key.a = 0;
                break;
            case SDL_SCANCODE_S:
                key.s = 0;
                break;
            case SDL_SCANCODE_D:
                key.d = 0;
                break;
            case SDL_SCANCODE_R:
                key.r = 0;
                break;

            case SDL_SCANCODE_UP:
                key.upArrow = 0;
                break;

            case SDL_SCANCODE_LEFT:
                key.leftArrow = 0;
                break;
            case SDL_SCANCODE_DOWN:
                key.downArrow = 0;
                break;
            case SDL_SCANCODE_RIGHT:
                key.rightArrow = 0;
                break;
            }
            break;
		}
	}

	SDL_GetWindowSize((&context)->Window, &w, &h);
	
		// issues a draw call
		float newTime = SDL_GetTicks();
		context.DeltaTime = newTime - lastTime;
		lastTime = newTime;

		Matrix4x4 cameraMatrix = Matrix4x4_CreateOrthographicOffCenter(
		0,
		w,
		h,
		0,
		0,
		-1
	);
	
    SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer((&context)->Device);
    if (cmdBuf == NULL)
    {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return -1;
    }

	SDL_GPUTexture* gpuRenderTarget;

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuf, (&context)->Window, &swapchainTexture, NULL, NULL)) {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return -1;
    }

	if (swapchainTexture != NULL)
	{
		// Build sprite instance transfer
		dataPtr = SDL_MapGPUTransferBuffer(
			(&context)->Device,
			SpriteComputeTransferBuffer,
			true
		);

		SDL_UnmapGPUTransferBuffer((&context)->Device, SpriteComputeTransferBuffer);

		// Upload instance data
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuf);
		SDL_UploadToGPUBuffer(
			copyPass,
			&(SDL_GPUTransferBufferLocation) {
				.transfer_buffer = SpriteComputeTransferBuffer,
				.offset = 0
			},
			&(SDL_GPUBufferRegion) {
				.buffer = SpriteComputeBuffer,
				.offset = 0,
				.size = SPRITE_COUNT * sizeof(ComputeSpriteInstance)
			},
			true
		);
		SDL_EndGPUCopyPass(copyPass);

		// Set up compute pass to build vertex buffer
		SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
			cmdBuf,
			NULL,
			0,
			&(SDL_GPUStorageBufferReadWriteBinding){
				.buffer = SpriteVertexBuffer,
				.cycle = true
			},
			1
		);

		count = 0;
		submitSprites(context.DeltaTime, w, h, quit, key);

		SDL_BindGPUComputePipeline(computePass, ComputePipeline);
		SDL_BindGPUComputeStorageBuffers(
			computePass,
			0,
			&(SDL_GPUBuffer*){
				SpriteComputeBuffer,
			},
			1
		);
		SDL_DispatchGPUCompute(computePass, SPRITE_COUNT / 64, 1, 1);

		SDL_EndGPUComputePass(computePass);

		// Render sprites
		SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
			cmdBuf,
			&(SDL_GPUColorTargetInfo){
				.texture = gpuRenderTarget,
				.cycle = false,
				.load_op = SDL_GPU_LOADOP_CLEAR,
				.store_op = SDL_GPU_STOREOP_STORE,
				.clear_color = {colorKey[0], colorKey[1], colorKey[2], colorKey[3]}
			},
			1,
			NULL
		);

		SDL_BindGPUGraphicsPipeline(renderPass, RenderPipeline);
		SDL_BindGPUVertexBuffers(
			renderPass,
			0,
			&(SDL_GPUBufferBinding){
				.buffer = SpriteVertexBuffer
			},
			1
		);
		SDL_BindGPUIndexBuffer(
			renderPass,
			&(SDL_GPUBufferBinding){
				.buffer = SpriteIndexBuffer
			},
			SDL_GPU_INDEXELEMENTSIZE_32BIT
		);
		SDL_BindGPUFragmentSamplers(
			renderPass,
			0,
			&(SDL_GPUTextureSamplerBinding){
				.texture = Texture,
				.sampler = Sampler
			},
			1
		);
		SDL_PushGPUVertexUniformData(
			cmdBuf,
			0,
			&cameraMatrix,
			sizeof(Matrix4x4)
		);
		SDL_DrawGPUIndexedPrimitives(
			renderPass,
			SPRITE_COUNT * 6,
			1,
			0,
			0,
			0
		);

		SDL_EndGPURenderPass(renderPass);
	}

	SDL_SubmitGPUCommandBuffer(cmdBuf);
	}

	SDL_ReleaseGPUComputePipeline((&context)->Device, ComputePipeline);
	SDL_ReleaseGPUGraphicsPipeline((&context)->Device, RenderPipeline);
	SDL_ReleaseGPUSampler((&context)->Device, Sampler);
	SDL_ReleaseGPUTexture((&context)->Device, Texture);
	SDL_ReleaseGPUTransferBuffer((&context)->Device, SpriteComputeTransferBuffer);
	SDL_ReleaseGPUBuffer((&context)->Device, SpriteComputeBuffer);
	SDL_ReleaseGPUBuffer((&context)->Device, SpriteVertexBuffer);
	SDL_ReleaseGPUBuffer((&context)->Device, SpriteIndexBuffer);

	CommonQuit(&context);

	return 0;
}
