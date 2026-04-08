#ifndef render_h
#define render_h

#include "engine.hpp"

// the vertex input layout

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

void renderingLoop(SDL_Window* win);

#endif