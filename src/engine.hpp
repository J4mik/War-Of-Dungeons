#pragma once

#include "collisions.hpp"
#include "fonts.hpp"
#include "level.hpp"


uint64_t lastTick;
uint16_t deltaTime;

struct
{
    int w = 1000;
    int h = 600;
    float ofsetX;
    float ofsetY;
} screen;

class sprite
{
public:
    float x;
    float y;
    uint16_t h;
    uint16_t w;
    float VectX;
    float VectY;
};

struct
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
} key;

uint16_t mouseX;
uint16_t mouseY;

bool running = 1;

void inputs()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            running = 0;
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

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            break;
        }
    }


    deltaTime = SDL_GetTicks() - lastTick;
    if (deltaTime > 250)
    {
        deltaTime = 0;
    }
    lastTick = SDL_GetTicks();
}
