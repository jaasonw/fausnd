set -v
g++ -I/c/mingw/sdl/include -L/c/mingw/sdl/lib fausnd.cpp -ggdb -std=c++11 -Wall -mconsole -lSDL2 -lSDL2_mixer -o fausnd
