set -v
g++ fausnd.cpp -I/c/mingw/sdl/include -L/c/mingw/sdl/lib -ggdb -std=c++11 -Wall -mconsole -lSDL2 -lSDL2_mixer -o fausnd
