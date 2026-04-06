cmake_minimum_required(VERSION 3.10)
project(WarOfDungeons
VERSION 1.0
DESCRIPTION "Pixel art platformer")

set(CMAKE_CPP_COMPILER g++)
set(CMAKE_MAKE_PROGRAM ninja)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# -Iinclude
include_directories(${CMAKE_SOURCE_DIR}/include)
# -Llib
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(WarOfDungeons
    main.c
    Common.h
    stb_image.h
    Common.c
    game.hpp
    game.cpp
    src/render.hpp
    src/render.cpp
    src/level.hpp
    src/level.cpp
    src/chunkgen.hpp
    src/chunkgen.cpp
    src/engine.hpp
    src/engine.cpp
)

set(SDL3_LIBRARIES mingw32 SDL3 SDL3_image SDL3_mixer SDL3_ttf)

target_link_libraries(WarOfDungeons PUBLIC ${SDL3_LIBRARIES})