#include <iostream>
#include <algorithm>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#undef main

#define DEBUG 0


struct sample
{
    Mix_Chunk * ptr;
    int id; // passed back to GM
};
std::vector<sample*> samples;

struct generator
{
    sample * sample_ptr;
    int channel; // channel being played back on
    int id; // passed back to GM
};
std::vector<generator*> generators;

int bottom_free_sample = 0;
int bottom_free_generator = 0;
int bottom_free_channel = 0;

int channel_count = 16;
std::vector<bool> channels_used;


int faudio_init()
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


int faudio_new_sample(const char * fnamestr)
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
        std::cout << "Could not open test file: " << Mix_GetError() << std::endl;
        return -1;
    }
    else
    {
        bottom_free_sample += 1;
        return mine->id;
    }
}


int faudio_kill_sample(int sid)
{
    for (unsigned index = 0; index < samples.size(); ++index)
    {
        if(samples[index]->id == sid)
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


int faudio_new_generator(int sid)
{
    auto mine = new generator;
    generators.push_back(mine);
    mine->id = bottom_free_generator;
    mine->channel = bottom_free_channel;
    
    bool valid = false;
    
    for (unsigned index = 0; index < samples.size(); ++index)
    {
        if(samples[index]->id == sid)
        {
            mine->sample_ptr = samples[index];
            valid = true;
        }
    }
    // no such sample
    if(!valid)
        return -1;
    
    while(bottom_free_channel > channel_count)
    {
        channel_count *= 2;
        Mix_AllocateChannels(channel_count);
        std::cout << "Max: " << channels_used.max_size() << " Want: " << channel_count << std::endl;
        try
        {
            channels_used.resize(channel_count, false);
        }
        catch(const std::exception &exc)
        {
            std::cout << "Caught exception: " << exc.what();
        }
        catch(...)
        {
            std::cout << "Caught unknown exception";
        }
        std::cout << "Bottom free: " << bottom_free_channel << " gen: " << bottom_free_generator << " size: " << channels_used.size() << std::endl;
    }
    channels_used[bottom_free_channel] = true;
    bottom_free_channel += 1;
    bottom_free_generator += 1;
    
    return mine->id;
}

int faudio_fire_generator(int gid)
{
    
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(generators[index]->id == gid)
        {
            Mix_PlayChannelTimed(generators[index]->channel, generators[index]->sample_ptr->ptr, 0, -1);
            return 0;
        }
    }
    return -1; // no such generator
}

int faudio_loop_generator(int gid)
{
    
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(generators[index]->id == gid)
        {
            Mix_PlayChannelTimed(generators[index]->channel, generators[index]->sample_ptr->ptr, -1, -1);
            return 0;
        }
    }
    return -1; // no such generator
}
int faudio_stop_generator(int gid)
{
    
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(generators[index]->id == gid)
        {
            Mix_HaltChannel(generators[index]->channel);
            return 0;
        }
    }
    return -1; // no such generator
}


int faudio_volume_generator(int gid, double amp)
{
    
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(generators[index]->id == gid)
        {
            Mix_Volume(generators[index]->channel, int(amp*MIX_MAX_VOLUME));
            return 0;
        }
    }
    return -1; // no such generator
}

int faudio_volumes_generator(int gid, double ampl , double ampr)
{
    
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(generators[index]->id == gid)
        {
            Mix_SetPanning(generators[index]->channel, int(ampl*MIX_MAX_VOLUME), int(ampr*MIX_MAX_VOLUME));
            return 0;
        }
    }
    return -1; // no such generator
}

int faudio_pan_generator(int gid, double pan) // 0 = center; -1 = left; 1 = right -- based on center volume
{
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(generators[index]->id == gid)
        {
            if(pan == 0.0) // compensate for center-less-ness of (127-x, 127+x) style
                Mix_SetPanning(generators[index]->channel, 127, 127);
            else if (pan > 0.0)
                Mix_SetPanning(generators[index]->channel, 127 - int(pan*128.0), 127 + int(pan*127.0));
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

//faudio_get_generator_pan
//faudio_get_generator_volume

int faudio_get_generator_playing(int gid)
{
    for (unsigned index = 0; index < generators.size(); ++index)
    {
        if(generators[index]->id == gid)
        {
            return Mix_Playing(generators[index]->channel);
        }
    }
    return -1; // no such generator
}


int faudio_kill_generator(int gid)
{
    for (size_t index = 0; index < generators.size(); ++index)
    {
        if(generators[index]->id == gid)
        {
            bottom_free_channel = bottom_free_channel < generators[index]->channel ? bottom_free_channel : generators[index]->channel;
            
            channels_used[generators[index]->channel] = false;
            
            /*auto top_used_generator = 0;
            
            for (int i = channels_used.size(); i > 0; --i)
            {
                if (channels_used[i] == true)
                {
                    top_used_generator = i;
                    break;
                }
            }
            if(top_used_generator < channel_count/3)
            {
                channel_count *= 2;
                Mix_AllocateChannels(channel_count);
                channels_used.resize(channel_count, false);
            }*/
            
            auto d = generators[index];
            generators.erase(generators.begin()+index);
            
            free(d);
            return 0;
        }
    }
    return -1; // Could not find generator by id
}


int main()
{
    faudio_init();
    
    auto smp = faudio_new_sample("test.wav");
    
    int gens[10000];
    for(int i = 0; i < 10000; i++)
        gens[i] = faudio_new_generator(smp);
    
    faudio_fire_generator(gens[1]);
    
    while(faudio_get_generator_playing(gens[1]))
        SDL_Delay(100);
    
    return 0;
}
