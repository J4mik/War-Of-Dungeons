cmake_minimum_required(VERSION 3.10)
project(WarOfDungeons)
find_package(SDL3 CONFIG QUIET)

if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found. Building it from source.")
    include(FetchContent)
    FetchContent_Declare(
            SDL
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG        "release-3.2.16"
    )
    FetchContent_MakeAvailable(SDL)
else(
    find_package(SDL3 REQUIRED))
endif()

add_executable(WarOfDungeons
    main.c
    Common.h
    stb_image.h
    Common.c
    ComputeSampler.c
    ComputeUniforms.c
    BasicCompute.c
    ComputeSpriteBatch.c
    PullSpriteBatch.c
)

target_link_libraries(WarOfDungeons
    SDL3::SDL3
)
