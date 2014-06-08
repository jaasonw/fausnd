set -v

g++ -std=c++11  fausnd.cpp -I/c/mingw/include -L/c/mingw/lib -std=c++11 -Wall -lSDL2_mixer -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -s -static -shared -o fausnd.dll
