#include <iostream>
#include <algorithm>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#undef main

#define DEBUG 1
#define DLLEXPORT extern "C" __declspec(dllexport)
#define EPSILON std::numeric_limits<double>::epsilon()
struct sample
{
    Mix_Chunk * ptr;
    double id; // passed back to GM
};
std::vector<sample*> samples;

struct generator
{
    sample * sample_ptr;
    unsigned int channel; // channel being played back on
    double id; // passed back to GM
};
std::vector<generator*> generators;

double bottom_free_sample = 0;
unsigned int bottom_free_generator = 0;
unsigned int bottom_free_channel = 0;

unsigned int channel_count = 16;
std::vector<bool> channels_used;

bool doubleComparison(double a, double b);

DLLEXPORT double faudio_init()
{
    // start SDL with audio support
    if(SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // open 44.1KHz, signed 16bit, system byte order,
    //      stereo audio, using 1024 byte chunks
    if(Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) != 0)
    {
        std::cout << "Could not open mixer: " << Mix_GetError() << std::endl;
        return -2;
    }
    #if DEBUG
    else
    {
        Uint16 f;
        Mix_QuerySpec(NULL, &f, NULL);
        std::cout << "Opened " << f << std::endl;
    }
    #endif
    Mix_AllocateChannels(channel_count);
    channels_used.resize(channel_count, false);

    return 0;
}


DLLEXPORT double faudio_new_sample(const char * fnamestr)
{
    auto mine = new sample;

    // here: set bottom free sample according to sorted search
    //

    mine->id = bottom_free_sample;
    mine->ptr = Mix_LoadWAV(fnamestr);


    // sorted insert mine into samples
    unsigned index;
    for (index = 0; index < samples.size(); ++index)
    {
        if(samples[index]->id > bottom_free_sample)
        {
            samples.insert(samples.begin()+index, mine);
            break;
        }
    }
    if (index >= samples.size())
        samples.push_back(mine);

    if(!mine->ptr)
    {
        std::cout << "Could not open file: " << Mix_GetError() << std::endl;
        return -1;
    }
    else
    {
        bottom_free_sample += 1;
        return mine->id;
    }
}


DLLEXPORT double faudio_kill_sample(double sid)
{
    for (unsigned index = 0; index < samples.size(); ++index)
    {
        if(doubleComparison(samples[index]->id, sid))
        {
            Mix_FreeChunk(samples[index]->ptr);
            auto d = samples[index];
            samples.erase(samples.begin()+index);

            free(d);
            return 0;
        }
    }
    return 1; // Could not find sample by id
}


DLLEXPORT double faudio_new_generator(double sid)
{
    // allocate a new generator
    auto mine = new generator;
    generators.push_back(mine);
    mine->id = bottom_free_generator;
    mine->channel = bottom_free_channel;

    bool valid = false;
    // check the given sample for validity
    for (unsigned index = 0; index < samples.size(); ++index)
    {
        if(doubleComparison(samples[index]->id, sid))
        {
            valid = true;
            mine->sample_ptr = samples[index];
        }
    }
    // no such sample
    if(!valid)
    {
        generators.pop_back();
        delete mine;
        return -1;
    }
    // reserve a mixer channel for our generator
    if(channels_used[bottom_free_channel])
        std::cout << "ERROR -- advance channel reservation not working! -- Generator" << mine->id << " Channel " << bottom_free_channel << "\n";
    channels_used[bottom_free_channel] = true;

    // expand mixer channel count if needed
    while(bottom_free_channel+1 >= channel_count)
    {
        channel_count *= 2;
        Mix_AllocateChannels(channel_count);
        std::cout << "Max: " << channels_used.max_size() << " Want: " << channel_count << std::endl;
        #if DEBUG
        try
        {
        #endif
            channels_used.resize(channel_count, false);
        #if DEBUG
        }
        catch(const std::exception &exc)
        {
            std::cout << "Caught exception: " << exc.what();
        }
        catch(...)
        {
            std::cout << "Caught unknown exception";
        }
        #endif
        std::cout << "Bottom free: " << bottom_free_channel << " gen: " << bottom_free_generator << " size: " << channels_used.size() << std::endl;
    }
    // set which mixer channel to reserve in advance
    for (unsigned i = floor(bottom_free_channel) + 1; i < channels_used.size(); ++i)
    {
        if(!channels_used[i])
        {
            bottom_free_channel = i;
            break;
        }
    }

    bottom_free_generator += 1;

    return mine->id;
}

DLLEXPORT double faudio_fire_generator(double gid)
{

    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            Mix_PlayChannelTimed(generators[index]->channel, generators[index]->sample_ptr->ptr, 0, -1);
            return 0;
        }
    }
    return -1; // no such generator
}

DLLEXPORT double faudio_loop_generator(double gid)
{

    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            Mix_PlayChannelTimed(generators[index]->channel, generators[index]->sample_ptr->ptr, -1, -1);
            return 0;
        }
    }
    return -1; // no such generator
}
DLLEXPORT double faudio_stop_generator(double gid)
{

    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            Mix_HaltChannel(generators[index]->channel);
            return 0;
        }
    }
    return -1; // no such generator
}


DLLEXPORT double faudio_volume_generator(double gid, double amp)
{

    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            Mix_Volume(generators[index]->channel, int(amp*MIX_MAX_VOLUME));
            return 0;
        }
    }
    return -1; // no such generator
}

DLLEXPORT double faudio_volumes_generator(double gid, double ampl , double ampr)
{

    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            Mix_SetPanning(generators[index]->channel, int(ampl*MIX_MAX_VOLUME), int(ampr*MIX_MAX_VOLUME));
            return 0;
        }
    }
    return -1; // no such generator
}

DLLEXPORT double faudio_pan_generator(double gid, double pan) // 0 = center; -1 = left; 1 = right -- based on center volume
{
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            if(doubleComparison(pan, 0.0)) // compensate for center-less-ness of (127-x, 127+x) style pan
                Mix_SetPanning(generators[index]->channel, 127, 127);
            else if (pan > 0.0)
                Mix_SetPanning(generators[index]->channel, 127 - int(pan*127.0), 127 + int(pan*127.0));
            else
                Mix_SetPanning(generators[index]->channel, 127 - int(pan*127.0), 127 + int(pan*127.0));

            #if DEBUG
            std::cout << "PANNED " << pan << std::endl;
            #endif
            return 0;
        }
    }
    return -1; // no such generator
}

DLLEXPORT double faudio_get_generator_playing(double gid)
{
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            return Mix_Playing(generators[index]->channel);
        }
    }
    return -1; // no such generator
}

DLLEXPORT double faudio_get_generator_volume(double gid)
{
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            //When volume is set to -1, then it returns the current volume of channel
            double volume = (double)Mix_Volume(generators[index]->channel, -1) / MIX_MAX_VOLUME;
            return volume;
        }
    }
    return -1; // no such generator
}


DLLEXPORT double faudio_kill_generator(double gid)
{
    for (size_t index = 0; index < generators.size(); ++index)
    {
        if(doubleComparison(generators[index]->id, gid))
        {
            bottom_free_channel = bottom_free_channel < generators[index]->channel ? bottom_free_channel : generators[index]->channel;

            channels_used[generators[index]->channel] = false;

            auto d = generators[index];
            generators.erase(generators.begin()+index);

            free(d);
            return 0;
        }
    }
    return -1; // Could not find generator by id
}

DLLEXPORT double faudio_kill_all_generators()
{
    #if DEBUG
    std::cout << "Size of generators before:  " << generators.size() << std::endl;
    std::cout << "Size of channels before:  " << channels_used.size() << std::endl;
    #endif
    for (int index = generators.size()-1; index >= 0; --index) //Go through the list of generators, and delete each one individually
    {
        Mix_HaltChannel(generators[index]->channel);
        auto d = generators[index];
        generators.pop_back();
        free(d);
    }
    channel_count = 16;
    bottom_free_generator = 0;
    bottom_free_channel = 0;
    while (channels_used.size() > channel_count){
        channels_used.pop_back();
    }
    //set all elements to false
    std::fill(channels_used.begin(), channels_used.end(), false);
    Mix_AllocateChannels(channel_count);
    #if DEBUG
    std::cout << "Size of generators after:  " << generators.size() << std::endl;
    std::cout << "Number of channels used:  " << channels_used.size() << std::endl;

    #endif
    return 0;
}

DLLEXPORT double faudio_kill_all_samples()
{
    #if DEBUG
    std::cout << "Size of samples before:  " << samples.size() << std::endl;
    #endif
    for (int index = samples.size()-1; index >= 0; --index)
    {
        Mix_FreeChunk(samples[index]->ptr);
        auto d = samples[index];
        free(d);
        samples.pop_back();
    }
    bottom_free_sample = 0;
    #if DEBUG
    std::cout << "Size of samples after:  " << samples.size() << std::endl;
    #endif
    return 0;
}

DLLEXPORT double faudio_shutdown()
{
    faudio_kill_all_generators();
    faudio_kill_all_samples();
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}
DLLEXPORT const char* faudio_get_error()
{
    return Mix_GetError();
}

bool doubleComparison(double a, double b)
{
    return fabs(a - b) < EPSILON;
}

int main()
{
    faudio_init();
    std::cout << "hello" << std::endl;
    auto smp = faudio_new_sample("C:/Users/mlin4_000/fausnd/test.wav");

    // make a shitload of generators to test allocation
    int gens[10000];
    for(int i = 0; i < 10000; i++)
        gens[i] = faudio_new_generator(smp);
    faudio_volume_generator(gens[1], 0.5);
    std::cout <<"volume:" << faudio_get_generator_volume(gens[1]) << std::endl;
    faudio_pan_generator(gens[1], 0.3);
    faudio_pan_generator(gens[2], -0.3);
    faudio_fire_generator(gens[1]);
    SDL_Delay(15);
    //faudio_fire_generator(gens[2]);
    // play a long sound to make sure sdl_mixer is stable
    //faudio_fire_generator(gens[2]);
    double sinWave = 0;
    while(faudio_get_generator_playing(gens[1])) {
        sinWave += 0.01;
        faudio_pan_generator(gens[1], sin(sinWave)/2+0.5);
        faudio_volume_generator(gens[2], sin(sinWave)/2+0.5);
        SDL_Delay(16.7);

    }
    faudio_shutdown();
    return 0;
}
