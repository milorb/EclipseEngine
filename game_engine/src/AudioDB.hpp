#ifndef AudioDB_hpp
#define AudioDB_hpp

#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <string>

#include "lua.hpp"
#include "LuaBridge.h"
#include "rapidjson/document.h"
#include "SDL2_mixer/SDL_mixer.h"
#include "AudioHelper.h"
#include "ComponentDB.hpp"

class AudioDB {
public:
    static void init();

    static inline std::unordered_map<std::string, Mix_Chunk*> clips;
    static void load_audio(std::string name);

    static void Play(float channel, std::string clip_name, bool looping);
    static void Halt(float channel);
    static void SetVolume(float channel, float volume);
};

#endif /* AudioDB_hpp */
