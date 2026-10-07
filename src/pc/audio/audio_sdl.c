/* audio_sdl.c - SDL2 output device for audio.h (48 kHz stereo s16). */
#include "audio.h"

#include <SDL.h>
#include <stdio.h>

static SDL_AudioDeviceID dev;

static void callback(void *user, Uint8 *stream, int len)
{
    (void)user;
    audio_mix((int16_t *)stream, len / 4);    /* SDL holds the device lock here */
}

int audio_open(void)
{
    SDL_AudioSpec want, have;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "audio: %s (running silent)\n", SDL_GetError());
        return -1;
    }
    SDL_zero(want);
    want.freq = AUDIO_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 1024;
    want.callback = callback;
    dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (!dev) {
        fprintf(stderr, "audio: %s (running silent)\n", SDL_GetError());
        return -1;
    }
    SDL_PauseAudioDevice(dev, 0);
    return 0;
}

int audio_device_open(void) { return dev != 0; }

void audio_close(void)
{
    if (dev)
        SDL_CloseAudioDevice(dev);
    dev = 0;
}

void audio_lock(void)
{
    if (dev)
        SDL_LockAudioDevice(dev);
}

void audio_unlock(void)
{
    if (dev)
        SDL_UnlockAudioDevice(dev);
}
