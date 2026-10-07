/*
 * audio.h - the port's audio interface (like gfx.h for graphics).
 *
 * A small software mixer (audio_mix.c, portable C) with two kinds of
 * sources, all mixed to 48 kHz stereo s16:
 *   - voices: one-shot or looping mono s16 samples (the SPU's job on the
 *     PS2: sound effects from the HD/BD packs), with volume, pan, pitch;
 *   - streams: stereo ring buffers the game thread fills (ADX music and
 *     ambience; CRI's ADXT on the PS2).
 * A backend owns the output device and calls audio_mix(): audio_sdl.c
 * (SDL2). An Xbox backend would be another file of the same size.
 * Every call from the game thread takes the backend's lock itself.
 */
#ifndef MH_AUDIO_H
#define MH_AUDIO_H

#include <stdint.h>

#define AUDIO_RATE    48000
#define AUDIO_VOICES  48        /* the PS2 SPU2 has 48 voices */
#define AUDIO_STREAMS 2         /* str_w channels 0 (BGM) and 1 (jingles) */

/* backend (audio_sdl.c) */
int  audio_open(void);          /* 0 = device open; -1 = silent (mixer still works) */
void audio_close(void);
void audio_lock(void);
void audio_unlock(void);

/* mixer (audio_mix.c) */
void audio_reset(void);
/* Start a voice; pcm must stay valid until the voice ends or is stopped.
 * loop = loop start sample or -1. vol 0..1, pan -1..1, pitch = rate
 * multiplier. Returns a voice id (> 0) or 0 if no voice is free. */
int  audio_voice_play(const int16_t *pcm, int n, int loop, int rate, float vol, float pan, float pitch);
int  audio_voice_set(int id, float vol, float pan, float pitch);  /* 0 if the voice ended */
void audio_voice_stop(int id);
int  audio_voice_playing(int id);
void audio_voice_stop_buffer(const int16_t *pcm, int n);         /* stop voices reading pcm[0..n) */
/* Streams: stereo frames at `rate`. */
int  audio_stream_free(int s);                                   /* frames that fit */
void audio_stream_write(int s, const int16_t *lr, int frames, int rate);
void audio_stream_clear(int s);
void audio_stream_vol(int s, float vol);
/* Reverb on the voices (sound effects; streams stay dry): wet 0..1 (0 =
 * off), size 0..1 (small room .. hall). A plain Schroeder reverb, not the
 * SPU2's reverb programs. */
void audio_reverb(float wet, float size);
/* Mix `frames` stereo frames into out (called by the backend, or by the
 * viewer's --audio-dump without a device). */
void audio_mix(int16_t *out, int frames);

#endif
