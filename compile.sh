set -v
g++ fausnd.cpp -I/c/mingw/sdl/include -L/c/mingw/sdl/lib -ggdb -std=c++11 -Wall -mconsole -lSDL2main -lSDL2 -lSDL2_mixer -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -s -o fausnd

