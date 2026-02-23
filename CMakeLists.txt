cmake_minimum_required(VERSION 3.15...3.31)
project(FlashSong VERSION 1.0
                  DESCRIPTION "Pixel art platformer"
                  LANGUAGES CXX)

set(CMAKE_CPP_COMPILER g++)
set(CMAKE_MAKE_PROGRAM ninja)

# for release
set(CMAKE_CXX_FLAGS) #-mwindows)

# sources
set(SOURCES main.cpp)# src/tiles.cpp)

# -Iinclude
include_directories(${CMAKE_SOURCE_DIR}/include)
# -Llib
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(FlashSong ${SOURCES})

# libraries to compile with -lSDL2main ...
set(SDL3_LIBRARIES mingw32 SDL3 SDL3_image SDL3_mixer SDL3_ttf)

# link it so ld can find it
target_link_libraries(FlashSong PUBLIC ${SDL3_LIBRARIES})

# target_sources(FlashSong PRIVATE resources.rc)