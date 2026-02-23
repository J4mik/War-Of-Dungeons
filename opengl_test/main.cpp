#include <cstdlib>

#define SDL_MAIN_HANDLED

#include "src/app.h"

int main(int argc, char **argv) {
    if (App app{}; app.start() < 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}