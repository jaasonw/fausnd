set -v
g++ -O fausnd.cpp -I/c/mingw/sdl/include -L/c/mingw/sdl/lib -ggdb -std=c++11 -Wall -lSDL2main -lSDL2_mixer -lSDL2 -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -s -static-libgcc -static -o fausnd.o

g++ -shared -o fausnd.dll fausnd.o

