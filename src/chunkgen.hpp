#pragma once

#define FREQUENCY1 0.004 // should be between 0.1 and 64
#define MULTIPLIER1 0.7
#define FREQUENCY2 0.02 // should be between 0.1 and 64
#define MULTIPLIER2 0.1
#define FREQUENCY3 0.001 // should be between 0.1 and 64
#define MULTIPLIER3 0.2

#define SPAGETTIFREQUENCY 0.006

#define CAVETRANSITIONFREQUENCY 0.0018

#define OCTAVES 5 // has to be between 1 and 16
#define SPAGHETTITHRESHOLD -0.82
#define SWISSCHEESETHRESHOLD 0.54
#define SEED 72

// generates values for spaghetti caves
double calculateSpaghettiCave(std::int32_t x, std::int32_t y);

// calculates values for swiss cheese caves
double calculateSwissCheeseCave(std::int32_t x, std::int32_t y);

// generates multipliers for the cave types
double calculateHeight(std::int32_t x, std::int32_t y);

int generateBiome(std::int32_t x, std::int32_t y, bool* tile);