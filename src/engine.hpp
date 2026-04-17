#ifndef engine_h
#define engine_h

#include <fstream>
#include <iostream>
#include <stdint.h>

// #include <math.h>

static uint64_t lastTick;
static uint16_t deltaTime;

struct screen
{
    int w;
    int h;
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
    unsigned int h;
    unsigned int w;
    float VectX;
    float VectY;
};

sprite player;

struct keyboard
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
} keyboard;

bool running = 1;

class expDecay
{
public:
    double pow255[5000] = {};

    void init()
    {
    std::basic_string<char> path = "./data/num.bin";

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

expDecay decay;

#endif