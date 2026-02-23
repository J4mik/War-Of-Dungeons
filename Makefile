all: compile link

compile:
	g++ -Iinclude -c main.cpp src/*.cpp

link:
	g++ *.o -o main -Llib -lmingw32 -lSDL2 -lSDL3_image -lSDL3_mixer -lSDL3_ttf -lSDL3_test