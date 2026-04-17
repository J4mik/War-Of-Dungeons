#ifndef renderAPI_h
#define renderAPI_h

#include <SDL3/SDL.h>

typedef struct Context
{
	const char* BasePath;
	SDL_Window* Window;
	SDL_GPUDevice* Device;
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

#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC SDL_malloc
#define STBI_REALLOC SDL_realloc
#define STBI_FREE SDL_free
#define STBI_ONLY_HDR

int CommonInit(Context* context, SDL_WindowFlags windowFlags, int windowX, int windowY);

void CommonQuit(Context* context);

SDL_GPUShader* LoadShader(
	SDL_GPUDevice* device,
	const char* shaderFilename,
	Uint32 samplerCount,
	Uint32 uniformBufferCount,
	Uint32 storageBufferCount,
	Uint32 storageTextureCount
);

SDL_GPUComputePipeline* CreateComputePipelineFromShader(
	SDL_GPUDevice* device,
	const char* shaderFilename,
	SDL_GPUComputePipelineCreateInfo *createInfo
);

SDL_Surface* LoadImage(const char* imageFilename, int desiredChannels);

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

// Matrix Math
Matrix4x4 Matrix4x4_CreateOrthographicOffCenter(
	float left,
	float right,
	float bottom,
	float top,
	float zNearPlane,
	float zFarPlane
);

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

static const Uint32 SPRITE_COUNT = 8912;

static float uCoords[4] = { 0.0f, 0.5f, 0.0f, 0.5f};
static float vCoords[4] = { 0.0f, 0.0f, 0.5f, 0.5f};

static float colorKey[4] = {1.0f, 0.0f, 1.0f, 1.0f};

typedef struct keyPress
{
    bool w;
    bool a;
    bool s;
    bool d;
    bool r;
    bool upArrow;
    bool leftArrow;
    bool downArrow;
    bool rightArrow;
} keyPress;

void renderSprite(ComputeSpriteInstance spriteRenderInstance);

void submitSprites(float dt, int w, int h, int quit, keyPress key);

int renderer();

#endif