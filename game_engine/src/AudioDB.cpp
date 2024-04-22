#include "AudioDB.hpp"

void AudioDB::init() {
    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginNamespace("Audio")
        .addFunction("Play", 
            &AudioDB::Play)
        .addFunction("Halt", 
            &AudioDB::Halt)
        .addFunction("SetVolume", &
            AudioDB::SetVolume)
        .endNamespace();
}

void AudioDB::load_audio(std::string name) {
    SDL_Init(SDL_INIT_AUDIO);
    
    std::string filepath= "resources/audio/" + name;
    int pathlen = static_cast<int>(filepath.size());
    if (std::filesystem::exists(filepath + ".wav") ) {
        filepath += ".wav";
    }
    if (std::filesystem::exists(filepath + ".ogg")) {
        filepath += ".ogg";
    }
    if (filepath.length() - 4 != pathlen) {
            std::cout << "error: failed to play audio clip " << name;
            std::exit(0);
    }
    
    if (AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0 ) {
        std::cout << "SDL failed to open audio";
        std::exit(0);
    }
    
    AudioHelper::Mix_AllocateChannels498(50);
    clips[name] = AudioHelper::Mix_LoadWAV498(filepath.c_str());
}

void AudioDB::Play(float _channel, std::string clip_name, bool _looping) {
    int channel = (int)_channel;
    int looping = 0;
    if (_looping) looping = -1;
    
    if (clips.find(clip_name) == clips.end()) {
        load_audio(clip_name);
    }
    
    Mix_Chunk* clip = clips[clip_name];

    AudioHelper::Mix_PlayChannel498(channel, clip, looping);
}

void AudioDB::Halt(float _channel) {
    int channel = (int)_channel;
    AudioHelper::Mix_HaltChannel498(channel);
}

void AudioDB::SetVolume(float _channel, float _volume) {
    int channel = (int)_channel;
    int volume = (int)_volume;
    AudioHelper::Mix_Volume498(channel, volume);
}
