#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

typedef struct Context
{
	const char* ExampleName;
	const char* BasePath;
	SDL_Window* Window;
	SDL_GPUDevice* Device;
	bool LeftPressed;
	bool RightPressed;
	bool DownPressed;
	bool UpPressed;
	float DeltaTime;
} Context;

// Vertex Formats
typedef struct PositionVertex
{
	float x, y, z;
} PositionVertex;

typedef struct PositionColorVertex
{
	float x, y, z;
	Uint8 r, g, b, a;
} PositionColorVertex;

typedef struct PositionTextureVertex
{
    float x, y, z;
    float u, v;
} PositionTextureVertex;

// Matrix Math
typedef struct Matrix4x4
{
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;
} Matrix4x4;

typedef struct Vector3
{
	float x, y, z;
} Vector3;

Matrix4x4 Matrix4x4_Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);
Matrix4x4 Matrix4x4_CreateRotationZ(float radians);
Matrix4x4 Matrix4x4_CreateTranslation(float x, float y, float z);
Matrix4x4 Matrix4x4_CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);
Matrix4x4 Matrix4x4_CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
Matrix4x4 Matrix4x4_CreateLookAt(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUpVector);
Vector3 Vector3_Normalize(Vector3 vec);
float Vector3_Dot(Vector3 vecA, Vector3 vecB);
Vector3 Vector3_Cross(Vector3 vecA, Vector3 vecB);


#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC SDL_malloc
#define STBI_REALLOC SDL_realloc
#define STBI_FREE SDL_free
#define STBI_ONLY_HDR
#include "stb_image.h"

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

	context->Window = SDL_CreateWindow(context->ExampleName, windowX, windowY, windowFlags);
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

static const char* BasePath = NULL;
void InitializeAssetLoader()
{
	BasePath = SDL_GetBasePath();
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

typedef struct ASTCHeader
{
	Uint8 magic[4];
	Uint8 blockX;
	Uint8 blockY;
	Uint8 blockZ;
	Uint8 dimX[3];
	Uint8 dimY[3];
	Uint8 dimZ[3];
} ASTCHeader;

typedef struct DDS_PIXELFORMAT {
	int dwSize;
	int dwFlags;
	int dwFourCC;
	int dwRGBBitCount;
	int dwRBitMask;
	int dwGBitMask;
	int dwBBitMask;
	int dwABitMask;
} DDS_PIXELFORMAT;

typedef struct DDS_HEADER {
	int dwMagic;
	int dwSize;
	int dwFlags;
	int dwHeight;
	int dwWidth;
	int dwPitchOrLinearSize;
	int dwDepth;
	int dwMipMapCount;
	int dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	int dwCaps;
	int dwCaps2;
	int dwCaps3;
	int dwCaps4;
	int dwReserved2;
} DDS_HEADER;

typedef struct DDS_HEADER_DXT10 {
  int dxgiFormat;
  int resourceDimension;
  unsigned int miscFlag;
  unsigned int arraySize;
  unsigned int miscFlags2;
} DDS_HEADER_DXT10;

void* LoadASTCImage(const char* imageFilename, int* pWidth, int* pHeight, int* pImageDataLength)
{
	char fullPath[256];
	SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Images/%s", BasePath, imageFilename);

	size_t fileSize;
	void* fileContents = SDL_LoadFile(fullPath, &fileSize);
	if (fileContents == NULL)
	{
		SDL_assert(!"Could not load ASTC image!");
		return NULL;
	}

	ASTCHeader* header = (ASTCHeader*)fileContents;
	if (header->magic[0] != 0x13 || header->magic[1] != 0xAB || header->magic[2] != 0xA1 || header->magic[3] != 0x5C)
	{
		SDL_assert(!"Bad magic number!");
		return NULL;
	}

	// Get the image dimensions in texels
	*pWidth = header->dimX[0] + (header->dimX[1] << 8) + (header->dimX[2] << 16);
	*pHeight = header->dimY[0] + (header->dimY[1] << 8) + (header->dimY[2] << 16);

	// Get the size of the texture data
	unsigned int block_count_x = (*pWidth + header->blockX - 1) / header->blockX;
	unsigned int block_count_y = (*pHeight + header->blockY - 1) / header->blockY;
	*pImageDataLength = block_count_x * block_count_y * 16;

	void* data = SDL_malloc(*pImageDataLength);
	SDL_memcpy(data, (char*)fileContents + sizeof(ASTCHeader), *pImageDataLength);
	SDL_free(fileContents);

	return data;
}

void* LoadDDSImage(const char* imageFilename, SDL_GPUTextureFormat format, int* pWidth, int* pHeight, int* pImageDataLength)
{
	char fullPath[256];
	SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Images/%s", BasePath, imageFilename);

	size_t fileSize;
	void* fileContents = SDL_LoadFile(fullPath, &fileSize);
	if (fileContents == NULL)
	{
		SDL_assert(!"Could not load DDS image!");
		return NULL;
	}

	DDS_HEADER* header = (DDS_HEADER*)fileContents;
	if (header->dwMagic != 0x20534444)
	{
		SDL_assert(!"Bad magic number!");
		return NULL;
	}

	bool hasDX10Header = header->ddspf.dwFlags == 0x4 && header->ddspf.dwFourCC == 0x30315844;

	*pWidth = header->dwWidth;
	*pHeight = header->dwHeight;
	*pImageDataLength = header->dwPitchOrLinearSize;

	void* data = SDL_malloc(*pImageDataLength);
	SDL_memcpy(data, (char*)fileContents + sizeof(DDS_HEADER) + (hasDX10Header ? sizeof(DDS_HEADER_DXT10) : 0), *pImageDataLength);
	SDL_free(fileContents);

	return data;
}

// Matrix Math

Matrix4x4 Matrix4x4_Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2)
{
	Matrix4x4 result;

	result.m11 = (
		(matrix1.m11 * matrix2.m11) +
		(matrix1.m12 * matrix2.m21) +
		(matrix1.m13 * matrix2.m31) +
		(matrix1.m14 * matrix2.m41)
	);
	result.m12 = (
		(matrix1.m11 * matrix2.m12) +
		(matrix1.m12 * matrix2.m22) +
		(matrix1.m13 * matrix2.m32) +
		(matrix1.m14 * matrix2.m42)
	);
	result.m13 = (
		(matrix1.m11 * matrix2.m13) +
		(matrix1.m12 * matrix2.m23) +
		(matrix1.m13 * matrix2.m33) +
		(matrix1.m14 * matrix2.m43)
	);
	result.m14 = (
		(matrix1.m11 * matrix2.m14) +
		(matrix1.m12 * matrix2.m24) +
		(matrix1.m13 * matrix2.m34) +
		(matrix1.m14 * matrix2.m44)
	);
	result.m21 = (
		(matrix1.m21 * matrix2.m11) +
		(matrix1.m22 * matrix2.m21) +
		(matrix1.m23 * matrix2.m31) +
		(matrix1.m24 * matrix2.m41)
	);
	result.m22 = (
		(matrix1.m21 * matrix2.m12) +
		(matrix1.m22 * matrix2.m22) +
		(matrix1.m23 * matrix2.m32) +
		(matrix1.m24 * matrix2.m42)
	);
	result.m23 = (
		(matrix1.m21 * matrix2.m13) +
		(matrix1.m22 * matrix2.m23) +
		(matrix1.m23 * matrix2.m33) +
		(matrix1.m24 * matrix2.m43)
	);
	result.m24 = (
		(matrix1.m21 * matrix2.m14) +
		(matrix1.m22 * matrix2.m24) +
		(matrix1.m23 * matrix2.m34) +
		(matrix1.m24 * matrix2.m44)
	);
	result.m31 = (
		(matrix1.m31 * matrix2.m11) +
		(matrix1.m32 * matrix2.m21) +
		(matrix1.m33 * matrix2.m31) +
		(matrix1.m34 * matrix2.m41)
	);
	result.m32 = (
		(matrix1.m31 * matrix2.m12) +
		(matrix1.m32 * matrix2.m22) +
		(matrix1.m33 * matrix2.m32) +
		(matrix1.m34 * matrix2.m42)
	);
	result.m33 = (
		(matrix1.m31 * matrix2.m13) +
		(matrix1.m32 * matrix2.m23) +
		(matrix1.m33 * matrix2.m33) +
		(matrix1.m34 * matrix2.m43)
	);
	result.m34 = (
		(matrix1.m31 * matrix2.m14) +
		(matrix1.m32 * matrix2.m24) +
		(matrix1.m33 * matrix2.m34) +
		(matrix1.m34 * matrix2.m44)
	);
	result.m41 = (
		(matrix1.m41 * matrix2.m11) +
		(matrix1.m42 * matrix2.m21) +
		(matrix1.m43 * matrix2.m31) +
		(matrix1.m44 * matrix2.m41)
	);
	result.m42 = (
		(matrix1.m41 * matrix2.m12) +
		(matrix1.m42 * matrix2.m22) +
		(matrix1.m43 * matrix2.m32) +
		(matrix1.m44 * matrix2.m42)
	);
	result.m43 = (
		(matrix1.m41 * matrix2.m13) +
		(matrix1.m42 * matrix2.m23) +
		(matrix1.m43 * matrix2.m33) +
		(matrix1.m44 * matrix2.m43)
	);
	result.m44 = (
		(matrix1.m41 * matrix2.m14) +
		(matrix1.m42 * matrix2.m24) +
		(matrix1.m43 * matrix2.m34) +
		(matrix1.m44 * matrix2.m44)
	);

	return result;
}

Matrix4x4 Matrix4x4_CreateRotationZ(float radians)
{
	return (Matrix4x4) {
		 SDL_cosf(radians), SDL_sinf(radians), 0, 0,
		-SDL_sinf(radians), SDL_cosf(radians), 0, 0,
						 0, 				0, 1, 0,
						 0,					0, 0, 1
	};
}

Matrix4x4 Matrix4x4_CreateTranslation(float x, float y, float z)
{
	return (Matrix4x4) {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	};
}

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

Matrix4x4 Matrix4x4_CreatePerspectiveFieldOfView(
	float fieldOfView,
	float aspectRatio,
	float nearPlaneDistance,
	float farPlaneDistance
) {
	float num = 1.0f / ((float) SDL_tanf(fieldOfView * 0.5f));
	return (Matrix4x4) {
		num / aspectRatio, 0, 0, 0,
		0, num, 0, 0,
		0, 0, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1,
		0, 0, (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance), 0
	};
}

Matrix4x4 Matrix4x4_CreateLookAt(
	Vector3 cameraPosition,
	Vector3 cameraTarget,
	Vector3 cameraUpVector
) {
	Vector3 targetToPosition = {
		cameraPosition.x - cameraTarget.x,
		cameraPosition.y - cameraTarget.y,
		cameraPosition.z - cameraTarget.z
	};
	Vector3 vectorA = Vector3_Normalize(targetToPosition);
	Vector3 vectorB = Vector3_Normalize(Vector3_Cross(cameraUpVector, vectorA));
	Vector3 vectorC = Vector3_Cross(vectorA, vectorB);

	return (Matrix4x4) {
		vectorB.x, vectorC.x, vectorA.x, 0,
		vectorB.y, vectorC.y, vectorA.y, 0,
		vectorB.z, vectorC.z, vectorA.z, 0,
		-Vector3_Dot(vectorB, cameraPosition), -Vector3_Dot(vectorC, cameraPosition), -Vector3_Dot(vectorA, cameraPosition), 1
	};
}

Vector3 Vector3_Normalize(Vector3 vec)
{
	float magnitude = SDL_sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
	return (Vector3) {
		vec.x / magnitude,
		vec.y / magnitude,
		vec.z / magnitude
	};
}

float Vector3_Dot(Vector3 vecA, Vector3 vecB)
{
	return (vecA.x * vecB.x) + (vecA.y * vecB.y) + (vecA.z * vecB.z);
}

Vector3 Vector3_Cross(Vector3 vecA, Vector3 vecB)
{
	return (Vector3) {
		vecA.y * vecB.z - vecB.y * vecA.z,
		-(vecA.x * vecB.z - vecB.x * vecA.z),
		vecA.x * vecB.y - vecB.x * vecA.y
	};
}
#include <SDL3/SDL_main.h>

static SDL_GPUComputePipeline* ComputePipeline;
static SDL_GPUGraphicsPipeline* RenderPipeline;
static SDL_GPUSampler* Sampler;
static SDL_GPUTexture* Texture;
static SDL_GPUTransferBuffer* SpriteComputeTransferBuffer;
static SDL_GPUBuffer* SpriteComputeBuffer;
static SDL_GPUBuffer* SpriteVertexBuffer;
static SDL_GPUBuffer* SpriteIndexBuffer;

typedef struct PositionTextureColorVertex
{
	float x, y, z, w;
	float u, v, padding_a, padding_b;
	float r, g, b, a;
} PositionTextureColorVertex;

typedef struct ComputeSpriteInstance
{
	float x, y, z;
	float rotation;
	float w, h, padding_a, padding_b;
	float tex_u, tex_v, tex_w, tex_h;
	float r, g, b, a;
} ComputeSpriteInstance;

static const Uint32 SPRITE_COUNT = 64;

static float uCoords[4] = { 0.0f, 0.5f, 0.0f, 0.5f};
static float vCoords[4] = { 0.0f, 0.0f, 0.5f, 0.5f};

static float colorKey[4] = {1.0f, 0.0f, 1.0f, 1.0f};

int main(int argc, char* argv[])
{
	Context context = { 0 };
	int quit = 0;
	float lastTime = 0;

	for (int i = 1; i < argc; i += 1)
	{
		if (SDL_strcmp(argv[i], "-name") == 0 && argc > i + 1)
		{
			const char* exampleName = argv[i + 1];
			int foundExample = 0;
		}
	}

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	InitializeAssetLoader();

	SDL_Gamepad* gamepad = NULL;
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
	SDL_Surface *imageData = LoadImage("data/images/player.png", 4);
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
		context.LeftPressed = 0;
		context.RightPressed = 0;
		context.DownPressed = 0;
		context.UpPressed = 0;

		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			if (evt.type == SDL_EVENT_QUIT)
			{
				quit = 1;
			}
			else if (evt.type == SDL_EVENT_KEY_DOWN)
			{
				if (evt.key.key == SDLK_D)
				{

				}
				else if (evt.key.key == SDLK_A)
				{

				}
				else if (evt.key.key == SDLK_LEFT)
				{
					context.LeftPressed = true;
				}
				else if (evt.key.key == SDLK_RIGHT)
				{
					context.RightPressed = true;
				}
				else if (evt.key.key == SDLK_DOWN)
				{
					context.DownPressed = true;
				}
				else if (evt.key.key == SDLK_UP)
				{
					context.UpPressed = true;
				}
			}
		}
		if (quit)
		{
			break;
		}


		// issues a draw call
		float newTime = SDL_GetTicks() / 1000.0f;
		context.DeltaTime = newTime - lastTime;
		lastTime = newTime;

		Matrix4x4 cameraMatrix = Matrix4x4_CreateOrthographicOffCenter(
		0,
		640,
		480,
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

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuf, (&context)->Window, &swapchainTexture, NULL, NULL)) {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return -1;
    }

	if (swapchainTexture != NULL)
	{
		// Build sprite instance transfer
		ComputeSpriteInstance* dataPtr = SDL_MapGPUTransferBuffer(
			(&context)->Device,
			SpriteComputeTransferBuffer,
			true
		);
		SDL_srand(0);


		for (Uint32 i = 0; i < SPRITE_COUNT; ++i)
		{
			Sint32 ravioli = SDL_rand(4);
			dataPtr[i].x = (float)(SDL_rand(640));
			dataPtr[i].y = (float)(SDL_rand(480));
			dataPtr[i].z = 0;
			dataPtr[i].rotation = 0;
			dataPtr[i].w = 32;
			dataPtr[i].h = 32;
			dataPtr[i].tex_u = uCoords[ravioli];
			dataPtr[i].tex_v = vCoords[ravioli];
			dataPtr[i].tex_w = 0.5f;
			dataPtr[i].tex_h = 0.5f;
			dataPtr[i].r = 1.0f;
			dataPtr[i].g = 1.0f;
			dataPtr[i].b = 1.0f;
			dataPtr[i].a = 1.0f;
		}

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
				.texture = swapchainTexture,
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
