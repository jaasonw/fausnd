fausnd - Faucet Audio
======
Faucet Audio is a WIP (early alpha) audio engine for extremely simple games that don't need special effects, but still want fine control over sound playback.

To be used in [Gang Garrison 2](http://ganggarrison.com/).


Dependencies
------------
Fausnd is distrubted as a self-contained GEX file. However, those that wish to extend the library will need the following dependencies
* [SDL2 (development library)](http://www.libsdl.org/download-2.0.php)
* [SDL2 Mixer](http://www.libsdl.org/projects/SDL_mixer/)

Building
---------
Fausnd requires the above dependencies. SDL2 and SDL2-Mixer should be statically linked. Fausnd should be exported as a dll file when packaged for release.

compile-dll.sh can be used to compile a dll version of fausnd. compile.sh can be used to compile a test application of fausnd. The corresponding directories to the SDL2 libraries and headers should be modified depending on your environment. Alternatively, a code blocks project is also included, but the relative paths of the headers and libraries must also be adjusted.

The dll must then be exported to a GEX file for use in a Game Maker project using [Extension Maker](sandbox.yoyogames.com/make/extensions).

Calling
-------
When calling each function, make sure to supply the correct amount of arguments and argument types. Game Maker doesn't actually type check when calling external functions, so make sure to do so or else Mark Overmars will hurt you.

Functions
---------
**real faudio_init()**

  *Initializes fausnd.*
  * Returns -1 if SDL failed to initialize
  * Returns -2 if SDL-Mixer failed to initialize
  * Returns 0 if initalized Successfully
  
  
**real faudio_new_sample(string filename)**

  *Creates a new sample.*
  * Returns -1 if unable to read file/file does not exist
  * Returns 0 otherwise
  
  
**real faudio_kill_sample(real sampleID)**

  *Frees a sound instance*
  * Returns 1 if unable to find sample instance
  * Returns 0 if successful
  
  
**real faudio_new_generator(real sampleID)**

  *Creates a new Generator. A generator is responsible for manipulating and modifying a sound instance.*
  * Returns -1 if sampleID is invalid
  * Returns generatorID if successful
  
  
**real faudio_fire_generator(real generatorID)**

  *Plays the Generator instance. Will stop playing after end of sample.*
  * Returns -1 if generatorID is invalid
  * Returns 0 if successful.
  
  
**real faudio_loop_generator(real generatorID)**

  *Plays a Generator instance that continuously loops through its sample.*
  * Returns -1 if generatorID is invalid.
  * Returns 0 if successful.
  
  
**real faudio_stop_generator(real generatorID)**

  *Stops the generator from playing*
  * Returns -1 is generatorID is invalid.
  * returns 0 if successful.
  
  
**real faudio_volume_generator(real generatorID, real volume)**

  *Sets the volume of the generator. Volume ranges from 0 (mute) to 1.0 (full volume).*
  * Returns -1 if generatorID is invalid.
  * returns 0 if successful.
  
  
**real faudio_pan_generator generatorID, real pan)**

  *Sets the pan of the generator. pan ranges from -1.0 (left) to 1.0 (right).*
  * Returns -1 if generatorID is invalid.
  * returns 0 if successful.
  
**real faudio_get_generator_volume (real generatorID)**

  *Returns the generator's current volume.*
  * Returns -1 if generatorID is invalid.
  * Returns a value from 0.0 to 1.0 indicating mute to full volume.

**real faudio_get_generator_playing (real generatorID)**

  *Returns whether or not the generator is playing.*
  * Returns -1 if generatorID is invalid.
  * returns 0 if not playing.
  * returns 1 if playing.
  
  
**real faudio_kill_generator(real generatorID)**

  *Frees the generator.*
  * returns -1 if generatorID is invalid.
  * returns 0 if successful.
  
  
  
License
-------
This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software and/or its source
code for any purpose, including commercial applications, and to alter
it and redistribute it freely, subject to the following conditions:

    1. The origin of the software must not be misrepresented.

        1a. Modified versions of the software or its source code must
        not be misrepesented as the original software or source code.

        1b. Acknowledgment of, attribution to, or notification to the
        developers of the used portion of the software is required for
        commercial use.

    2. This notice may not be removed from, or altered in, any source
    distribution of the software.

        2a. This notice is not required to be included in any compiled
        distribution of the software, unless said distribution also
        constitutes a source distribution.
