#include "src\gameloop.hpp"

int level = 1;

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO || SDL_INIT_AUDIO);
    TTF_Init();
    MIX_Init();

    SDL_Window* win = SDL_CreateWindow("FlashSong", screen.w, screen.h, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_Renderer* rend = SDL_CreateRenderer(win, NULL);
    SDL_SetRenderDrawColor(rend, 2, 15, 35, 255);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);


    SDL_Texture* playButton = IMG_LoadTexture(rend, "data/images/play.png");
    SDL_SetTextureScaleMode(playButton, SDL_SCALEMODE_NEAREST);

    SDL_FRect playButtonPos;
    Text ByteBounce("data/fonts/ByteBounce.ttf", 80);

    // while (!(mouseX > ((screen.w - 96) / 2) && mouseX < ((screen.w + 96) / 2) && mouseY > (screen.h / 2) && mouseY <
    // (screen.h / 2 + 36)) && running) {
    inputs();
    SDL_GetWindowSizeInPixels(win, &screen.w, &screen.h);
    SDL_RenderClear(rend);

    SDL_RenderTexture(rend, playButton, NULL, &playButtonPos);

    SDL_RenderPresent(rend);
    // SDL_Delay(10);
    // }

    playButtonPos = {0, 0, 0, 0};

    while (game(level, win, rend))
    {
        ++level;
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    TTF_Quit();
    MIX_Quit();
    return 0;
}
