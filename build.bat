glslc -fshader-stage=fragment shaders/fragment.glsl -o shaders/fragment.spv
glslc -fshader-stage=vertex shaders/vertex.glsl -o shaders/vertex.spv

cmake -S . -B build -G Ninja 
cd build
ninja -j10
cd ..
.\build\WarOfDungeons