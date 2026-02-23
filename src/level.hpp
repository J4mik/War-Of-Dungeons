#include <fstream>

#include <cmath>
#include <cstdint>

// int16_t tiles[0][2];

class tile {
    public:
    int16_t x;
    int16_t y;
    int16_t type;
};

class file {
   public:
    std::ifstream inputFileStream;
    int16_t length;
    uint16_t fileSize;
    bool error;
    int load(std::string path, uint16_t classSize) {
        try {
            error = 1;
            inputFileStream.open(path, std::ios::in|std::ios::binary);
            if (!inputFileStream.is_open()) {
                inputFileStream.close();
                return 0;
            }
            inputFileStream.seekg(0, std::fstream::end);
            fileSize = int(inputFileStream.tellg()) + 1;
            length = int(fileSize / classSize); // the 6 is for the size of "struct" [int16_t, int16_t, int16_t] this can be replaced if a different struct is used
            inputFileStream.seekg(0, std::fstream::beg);
            error = 0;
            return length;  
        }
        catch (std::ifstream::failure& error) {
            return 0;
        }
    }
    void read(tile (&arr)[]) {
        for (int i = 0; i <= length; ++i) {
            try {
                error = 1;
                inputFileStream.read((char*) &arr[i].x, 2); // if doesn't work try: sizeof(uint16_t) instead of 2
                inputFileStream.read((char*) &arr[i].y, 2); // if doesn't work try: sizeof(uint16_t) instead of 2
                inputFileStream.read((char*) &arr[i].type, 2); // if doesn't work try: sizeof(uint16_t) instead of 2
                error = 0;
            }
            catch (std::ifstream::failure& error) {
                return;
            }
        }
    }
    void close() {
        inputFileStream.close();
    }
};