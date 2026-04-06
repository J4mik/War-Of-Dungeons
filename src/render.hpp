#include "level.hpp"

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

void renderingLoop(SDL_Window* win);

SDL_Texture* tempTexture;

SDL_Texture* grass;

SDL_Texture* livelyGrass;

SDL_Texture* darkGrass;

SDL_Texture* driedGrass;

SDL_Texture* ice;

SDL_Texture* dirt;

SDL_Texture* snow;

SDL_Texture* sand;

SDL_Texture* redSand;

SDL_Texture* stone;

SDL_Texture* playerTexture;