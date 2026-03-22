cmake_minimum_required(VERSION 3.15...3.31)
project(WarOfDots VERSION 1.0
                  DESCRIPTION "Pixel art platformer"
                  LANGUAGES CXX)

set(CMAKE_CPP_COMPILER g++)
set(CMAKE_MAKE_PROGRAM ninja)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# for release
set(CMAKE_CXX_FLAGS) #-mwindows)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/data/images/logo.rc")

# sources
set(SOURCES main.cpp ${APP_ICON_RESOURCE_WINDOWS})

# -Iinclude
include_directories(${CMAKE_SOURCE_DIR}/include)
# -Llib
link_directories(${CMAKE_SOURCE_DIR}/lib)


add_executable(WarOfDots ${SOURCES})

# libraries to compile with -lSDL2main ...
set(SDL3_LIBRARIES mingw32 SDL3 SDL3_image SDL3_mixer SDL3_ttf)

# link it so ld can find it
target_link_libraries(WarOfDots PUBLIC ${SDL3_LIBRARIES})

# target_sources(WarOfDots PRIVATE resources.rc)