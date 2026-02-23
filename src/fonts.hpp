#include <SDL3/SDL_ttf.h>

#define white SDL_Color{255, 255, 255, 255}
#define gray SDL_Color{100, 100, 100, 255}
#define black SDL_Color{0, 0, 0, 255}
#define red SDL_Color{255, 0, 0, 255}
#define orange SDL_Color{255, 127, 0, 255}
#define yellow SDL_Color{255, 255, 0, 255}
#define green SDL_Color{0, 255, 0, 255}
#define cyan SDL_Color{20, 200, 255, 255}
#define blue SDL_Color{0, 50, 255, 255}
#define magenta SDL_Color{255, 0, 220, 255}
#define purple SDL_Color{180, 0, 255, 255}

class Text {
    TTF_Font* font;
    public:
        SDL_Surface* fontSurface;
        Text(const char *path, uint16_t size) {
            font = TTF_OpenFont(path, size);
        }
        void render_toSurface(SDL_Color fontColor, const char *text) {
            fontSurface = SDL_CreateSurface(100, 100, SDL_PIXELFORMAT_RGBA32);
            fontSurface = TTF_RenderText_Solid(font, text, 0, fontColor);
        }
        ~Text() {
            TTF_CloseFont(font);
            SDL_DestroySurface(fontSurface);
        }
};