/* audio_sdl.c - SDL2 output device for audio.h (48 kHz stereo s16). */
#include "audio.h"
#include "../rt/rt_log.h"

#include <SDL.h>
#include <stdio.h>

static SDL_AudioDeviceID dev;

static void callback(void *user, Uint8 *stream, int len)
{
    /* the debug log's underrun check: the time since the last call and the mix time, against the buffer's length */
    static Uint64 last;
    Uint64 t0 = SDL_GetPerformanceCounter(), f = SDL_GetPerformanceFrequency();
    (void)user;
    audio_mix((int16_t *)stream, len / 4);    /* SDL holds the device lock here */
    {
        Uint64 t1 = SDL_GetPerformanceCounter();
        rt_log_audio_cb(last ? (double)(t0 - last) * 1000.0 / (double)f : 0.0, (double)(t1 - t0) * 1000.0 / (double)f,
                        (int)((len / 4) * 1000 / AUDIO_RATE));
        last = t0;
    }
}

int audio_open(void)
{
    SDL_AudioSpec want, have;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "audio: %s (running silent)\n", SDL_GetError());
        rt_warn("audio: cannot start the audio subsystem: %s (running silent)", SDL_GetError());
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
        rt_warn("audio: cannot open an audio device: %s (running silent)", SDL_GetError());
        return -1;
    }
    rt_log("audio: driver %s, device \"%s\", %d Hz %d ch, buffer %d samples", SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver() : "?",
           SDL_GetNumAudioDevices(0) > 0 ? SDL_GetAudioDeviceName(0, 0) : "default", have.freq, have.channels, have.samples);
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
