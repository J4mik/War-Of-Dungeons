glslc -fshader-stage=fragment shaders/fragment.glsl -o shaders/fragment.spv
glslc -fshader-stage=vertex shaders/vertex.glsl -o shaders/vertex.spv

cmake -S . -B bin -G Ninja 
cd bin
ninja -j4
cd ..
.\bin\WarOfDungeons