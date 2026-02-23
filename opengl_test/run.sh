cmake -S . -B build -G Ninja
cd build; ninja -j 14; cd ..
./build/main