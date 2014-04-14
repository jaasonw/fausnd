#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_loadso.h>
#undef main

int main()
{
    std::cout << "Testing console output.\n";
    
    void * mydll = SDL_LoadObject("fausnd.dll");
    std::cout << SDL_GetError();
    
    double (*faudio_init)();
    faudio_init = (double(*)()) SDL_LoadFunction(mydll, "faudio_init");
    std::cout << SDL_GetError();
    
    double (*faudio_new_sample)(const char * sfn);
    faudio_new_sample = (double(*)(const char *)) SDL_LoadFunction(mydll, "faudio_new_sample");
    std::cout << SDL_GetError();
    
    double (*faudio_new_generator)(double sid);
    faudio_new_generator = (double(*)(double)) SDL_LoadFunction(mydll, "faudio_new_generator");
    std::cout << SDL_GetError();
    
    double (*faudio_fire_generator)(double gid);
    faudio_fire_generator = (double(*)(double)) SDL_LoadFunction(mydll, "faudio_fire_generator");
    std::cout << SDL_GetError();
    
    double (*faudio_get_generator_playing)(double gid);
    faudio_get_generator_playing = (double(*)(double)) SDL_LoadFunction(mydll, "faudio_get_generator_playing");
    std::cout << SDL_GetError();
    
    
    faudio_init();
    auto smp = faudio_new_sample("test.wav");
    auto gen = faudio_new_generator(smp);
    
    faudio_fire_generator(gen);
    while(faudio_get_generator_playing(gen))
        SDL_Delay(10);
    
    return 0;
}