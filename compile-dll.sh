set -v
g++ fausnd.cpp -I/c/mingw/sdl/include -L/c/mingw/sdl/lib -std=c++11 -Wall -lSDL2_mixer -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -s -static-libgcc -static -shared -o fausnd.dll

