#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_shadercross.h>
#include <fstream>
#include <iostream>


uint64_t lastTick;
uint16_t deltaTime;

struct
{
    int w = 1200;
    int h = 700;
    float ofsetX;
    float ofsetY;
    float posX;
    float posY;
    int tempOfsetX;
    int tempOfsetY;
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

sprite player;

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

class expDecay
{
public:
    double pow255[5000] = {};

    void init(std::basic_string<char> path)
    {

        std::ifstream FileStream;
        FileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            FileStream.open(path, std::ios::in | std::ios::binary);
            for (int i = 0; i < 5000; ++i)
            {
                FileStream.read((char*)&pow255[i], 8);
            }
            FileStream.close();
        }
        catch (std::ifstream::failure& error)
        {
            std::cout << "Error";
            if (!pow255[0] == 1)
            {
                std::cout << " reading from file '" << path << "', recreating file contents\n";
                std::ofstream FileStream;
                FileStream.open(path, std::ios::out | std::ios::binary);
                pow255[0] = 1;
                FileStream.write((char*)&pow255[0], 8);
                for (int i = 1; i < 5000; ++i)
                {
                    pow255[i] = pow255[i - 1] * 0.995;
                    FileStream.write((char*)&pow255[i], 8);
                }
                FileStream.close();
            }
        }
    }
};

void inputs();