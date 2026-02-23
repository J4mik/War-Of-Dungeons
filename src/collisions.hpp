#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_timer.h>
#include <iostream>

bool colidetect(SDL_FRect a, SDL_FRect b) {
    if ((b.x < a.x && a.x < (b.x + b.w)) || (b.x < (a.x + a.w) && (a.x + a.w) < (b.x + b.w))) {
        if ((b.y < a.y && a.y < (b.y + b.h)) || (b.y < (a.y + a.h) && (a.y + a.h) < (b.y + b.h))) {
            return 1;
        }
    }
    return 0;
}