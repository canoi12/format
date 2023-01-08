#include <SDL2/SDL.h>

#define WAV_IMPLEMENTATION
#include "../wav.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: ./wav [filename]\n");
        return 0;
    }
    wav_t wav;
    if (wav_load_file(argv[1], &wav) < 0) return 0;
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec spec;
    spec.freq = wav.header.sample_rate;
    spec.format = AUDIO_S16SYS;
    spec.channels = wav.header.channels;
    spec.samples = 4096;
    spec.callback = NULL;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    SDL_PauseAudioDevice(dev, 0);

    SDL_QueueAudio(dev, wav.data, wav.data_size);

    while (SDL_GetQueuedAudioSize(dev) > 0) SDL_Delay(100);

    wav_free(&wav);
    SDL_CloseAudioDevice(dev);
    SDL_Quit();

    return 0;
}