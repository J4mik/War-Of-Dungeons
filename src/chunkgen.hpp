#pragma once

#include "../include/perlin/perlin.hpp"


#define FREQUENCY1 0.004 // should be between 0.1 and 64
#define MULTIPLIER1 0.7
#define FREQUENCY2 0.02 // should be between 0.1 and 64
#define MULTIPLIER2 0.15
#define FREQUENCY3 0.001 // should be between 0.1 and 64
#define MULTIPLIER3 0.15

#define SPAGETTIFREQUENCY 0.006

#define CAVETRANSITIONFREQUENCY 0.0018

#define OCTAVES 5 // has to be between 1 and 16
#define SPAGHETTITHRESHOLD -0.82
#define SWISSCHEESETHRESHOLD 0.54
#define SEED 72

// generates values for spaghetti caves
double calculateSpaghettiCave(std::int32_t x, std::int32_t y)
{
    const siv::PerlinNoise perlin{SEED + 1};

    return -pow(1 - std::abs(0.5 - perlin.octave2D_01((x * SPAGETTIFREQUENCY), (y * SPAGETTIFREQUENCY), 5)), 2);
}

// calculates values for swiss cheese caves
double calculateSwissCheeseCave(std::int32_t x, std::int32_t y)
{
    const siv::PerlinNoise perlin{SEED};

    return perlin.octave2D_01((x * FREQUENCY1), (y * FREQUENCY1 + 8590), OCTAVES) * MULTIPLIER1 +
        perlin.octave2D_01((x * FREQUENCY2), (y * FREQUENCY2), OCTAVES) * MULTIPLIER2 +
        perlin.octave2D_01((x * FREQUENCY3 + 59475), (y * FREQUENCY3), OCTAVES) * MULTIPLIER3;
}

int generateBiome(std::int32_t x, std::int32_t y)
{
    const siv::PerlinNoise perlin{SEED + 3};

    double humidity;
    double temperature = 0.7;
    double weirdness;
    double liveliness;

    humidity = perlin.octave2D_01(x * 0.0017, y * 0.0017, 6);
    temperature = perlin.octave2D_01(x * 0.0007 + 6769, y * 0.0007 + 42069, 6);
    weirdness = perlin.octave2D_01(x * 0.002 + 69420, y * 0.002 + 61678, 6);

    if (temperature > 0.6)
    {
        if (humidity < 0.38)
        {
            if (weirdness < 0.67)
            {
                return 1;
                // desert
            }
            else
            {
                return 6;
                // messa
            }
        }
        else if (humidity < 0.63)
        {
            return 0;
            // savannah
        }
        else
        {
            return 2;
            // jungle
        }
    }
    else if (temperature > 0.4)
    {
        if (humidity < 0.38)
        {
            return 3;
            // stone lands
        }
        if (humidity < 0.63)
        {
            return 4;
            // plains
        }
        else
        {
            return 5;
            // swamp
        }
    }
    else
    {
        if (humidity < 0.38)
        {
            return 7;
            // snow lands
        }
        else if (humidity > 0.63)
        {
            return 8;
            // frozen lake
        }
        return 9;
        // thundra
    }
    return 0;
}

// generates multipliers for the cave types
bool calculateTile(std::int32_t x, std::int32_t y)
{
    const siv::PerlinNoise perlin{SEED + 2};

    double multiplier =
        perlin.octave2D_01((x * CAVETRANSITIONFREQUENCY), (y * CAVETRANSITIONFREQUENCY), OCTAVES) - 0.15;

    double value = multiplier * calculateSpaghettiCave(x, y) + (1 - multiplier) * calculateSwissCheeseCave(x, y);

    double threshold = (1 - multiplier) * SWISSCHEESETHRESHOLD + multiplier * SPAGHETTITHRESHOLD;

    return value > threshold;
}
