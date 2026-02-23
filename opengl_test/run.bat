cmake -S . -B build -G Ninja
cd build
ninja -j14 
cd..
./build/main