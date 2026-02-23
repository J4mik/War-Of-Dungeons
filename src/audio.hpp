#include <SDL3/SDL_mixer.h>
#include <thread>


    MIX_Mixer* mixer;
    MIX_Audio* music;
    MIX_Audio* fall;
    bool musicRunning = 1;
    void load() {
        SDL_AudioSpec audioSpec;
        audioSpec.format = SDL_AUDIO_F32;
        audioSpec.channels = 2;
        audioSpec.freq = 44100;
        mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
        music = MIX_LoadAudio(mixer, "data/audio/level.wav", 1);
        fall = MIX_LoadAudio(mixer, "data/audio/fall.wav", 1);
    }
    void musicThread() {
        MIX_PlayAudio(mixer, music);
    }
