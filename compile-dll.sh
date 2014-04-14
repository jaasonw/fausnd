set -v
g++ fausnd.cpp -I/c/mingw/sdl2/include -L/c/mingw/lib -ggdb -std=c++11 -Wall -lSDL2main -lSDL2_mixer -lSDL2 -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -s -static-libgcc -static -o fausnd.o

g++ -shared -o fausnd.dll fausnd.o

