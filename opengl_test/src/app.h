//
// Created by jenskromdijk on 14/03/25.
//

#ifndef APP_H
#define APP_H

#include <glad/glad.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "constants.hpp"
#include "shader.hpp"

class App {
public:
    App() = default;

    ~App() {
        if (!m_closed) {
            close();
        }
    }

    int start() {
        if (!init()) {
            std::cerr << "GAME::ERROR Failed to initialize!" << std::endl;
            return -1;
        }
        if (!loadMedia()) {
            std::cerr << "GAME::ERROR Failed to load media!" << std::endl;
            return -1;
        }
        run();
        close();
        return 0;
    }

    bool init() {
        std::cout << "GAME::INITIALIZING...\n";

        // ------------- INITIALIZE SDL & Subsystems ------------- //
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            std::cout << "GAME::INIT::ERROR SDL_Init: " << SDL_GetError() << std::endl;
            return false;
        }

        // initialize SDL_Image
        int imgFlags {IMG_INIT_PNG};
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            std::cout << "GAME::INIT::ERROR IMG_Init: " << IMG_GetError() << std::endl;
            return false;
        }

        // initialize SDL_TTF
        if (TTF_Init() == -1) {
            std::cout << "GAME::INIT::ERROR TTF_Init: " << TTF_GetError() << std::endl;
            return false;
        }

        if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1)
        {
            std::cout << "GAME::INIT::ERROR Mix_OpenAudio: " << Mix_GetError() << std::endl;
            return false;
        }

        // --------------- INITIALIZE SDL components -------------- //

        m_window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if (m_window == nullptr) {
            std::cout << "GAME::INIT::ERROR Failed to create SDL_Window! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // set up opengl
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // create opengl context
        m_context = SDL_GL_CreateContext(m_window);
        if (m_context == nullptr)
        {
            std::cout << "GAME::INIT::ERROR Failed to create OpenGL context!" << std::endl;
            return false;
        } else {
            // initialize glad
            if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
            {
                std::cout << "GAME::INIT::ERROR Failed to initialize GLAD!" << std::endl;
                return false;
            }

            std::cout << "GAME::INIT Successfully initialized GLAD!" << std::endl;
        }

        std::cout << "GAME::INIT::OK!" << std::endl;
        return true;
    }

    // load game assets
    bool loadMedia() {
        std::cout << "GAME::LOADING...\n";
        std::cout << "GAME::LOADING::OK!" << std::endl;
        return true;
    }

    // destroy renderer & window and quit SDL subsystems
    void close() {
        if (!m_closed) {
            std::cout << "GAME::CLOSING...\n";
            SDL_DestroyWindow(m_window);
            TTF_Quit();
            IMG_Quit();
            Mix_Quit();
            SDL_Quit();
            std::cout << "GAME::CLOSED!\n";
            m_closed = true;
        }
    }

    void run() {
        SDL_Event event;
        bool running {true};

        do {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_MOUSEMOTION)
                {
                    SDL_GetGlobalMouseState(&m_mouseX, &m_mouseY);
                    m_mouseX -= m_windowX;
                    m_mouseY -= m_windowY;
                } else if (event.type == SDL_WINDOWEVENT)
                {
                    // handle window events
                    m_width = event.window.data1 / 3;
                    m_height = event.window.data2 / 3;
                } else if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.keysym.sym = SDLK_ESCAPE)
                    {
                        running = false;
                    }
                }
            }

            SDL_GetWindowPosition(m_window, &m_windowX, &m_windowY);

            // update framebuffer
            glViewport(0, 0, m_width, m_height);

            glClearColor(1.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            SDL_GL_SwapWindow(m_window);

        } while (running);
    }

    [[nodiscard]] int getWidth() const {
        return m_width;
    }

    [[nodiscard]] int getHeight() const {
        return m_height;
    }

    [[nodiscard]] int getMouseX() const {
        return m_mouseX;
    }

    [[nodiscard]] int getMouseY() const {
        return m_mouseY;
    }

    [[nodiscard]] int getWindowX() const {
        return m_windowX;
    }

    [[nodiscard]] int getWindowY() const {
        return m_windowY;
    }

private:
    SDL_Window* m_window {nullptr};
    SDL_GLContext m_context{};

    bool m_closed{false};

    int m_width {WINDOW_WIDTH};
    int m_height {WINDOW_HEIGHT};
    int m_mouseX {0};
    int m_mouseY {0};
    int m_windowX {0};
    int m_windowY {0};
};

#endif //APP_H
