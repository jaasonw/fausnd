set -v

g++ -std=c++11 -std=c++0x -Wall -O2 -I/c/mingw/sdl/include -c fausnd.cpp -o fausnd.o
g++ -shared -Wl,--dll -L/c/mingw/sdl/lib fausnd.o -o fausnd.dll -lSDL2main -lSDL2_mixer -lSDL2 -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -static

