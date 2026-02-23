cmake_minimum_required(VERSION 3.15...3.31)

project(OpenglTest
                VERSION 1.0
                LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath lib/ -lGL -lGLU")

set(BIN_NAME main)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src/extern/)
link_directories(${CMAKE_SOURCE_DIR}/lib)

set(SOURCES main.cpp)

#g++ main.cpp src/extern/glad.c -o main -Iinclude -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf        
add_executable(${BIN_NAME} ${SOURCES})

set(SDL2_LIBRARIES SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf)
target_link_libraries(${BIN_NAME} PUBLIC GL GLU ${SDL2_LIBRARIES})
