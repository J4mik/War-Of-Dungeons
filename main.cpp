extern "C" 
{
#include "renderAPI.h"
}

#include "src/game.hpp"

#include <SDL3/SDL_main.h>
#include <thread>

void submitSprites(float dt, int w, int h, int quit, keyPress key) 
{
    keyboard = {key.w, key.a, key.s, key.d, key.r, key.upArrow, key.leftArrow, key.downArrow, key.rightArrow};
    deltaTime = dt;
    screen.w = w; screen.h = h;
    running = !quit;
    spriteRenderCall();
}

void spriteRender(spriteInstance spriteData) 
{
    renderSprite(ComputeSpriteInstance{
    spriteData.x, spriteData.y, spriteData.z, 
    spriteData.rotation,
    spriteData.w, spriteData.h, spriteData.padding_a, spriteData.padding_b,
    spriteData.tex_u, spriteData.tex_v, spriteData.tex_w, spriteData.tex_h,
    spriteData.r, spriteData.g, spriteData.b, spriteData.a});
}

int main(int argc, char* argv[])
{
    std::thread t1(game);
    std::thread t2(loadChunks);
    renderer();
    running = 0;
    t1.join();
    t2.join();
    return 0;
}