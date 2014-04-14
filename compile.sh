set -v
g++ fausnd.cpp -I/c/mingw/include/sdl2 -L/c/mingw/lib -ggdb -std=c++11 -Wall -mconsole -lSDL2main -lSDL2 -lSDL2_mixer -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -static -s -o fausnd

