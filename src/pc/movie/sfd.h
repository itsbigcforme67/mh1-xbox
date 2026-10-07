/*
 * sfd.h - Sofdec (.sfd) movie player core: an MPEG-1 program-stream demux
 * (the disc's .sfd files are MPEG-1 system streams of 2048-byte packs),
 * libmpeg2 for the MPEG-2 video (third_party/libmpeg2, plain C) and the
 * CRI ADX decoder of snd.c for the stream 0xC0 audio. Portable C: no
 * platform calls, so the Xbox build uses it unchanged.
 */
#ifndef MH_SFD_H
#define MH_SFD_H

#include <stdint.h>
#include "../fmt/fmt.h"

typedef struct sfd sfd;

/* Open AFS entry idx (the movie file) of afs. NULL if it is not a movie. */
sfd *sfd_open(const fmt_afs *afs, int idx);
void sfd_close(sfd *s);

/* Video: decode the next frame in display order. 1 = a frame is ready,
 * 0 = end of the stream. */
int  sfd_next_frame(sfd *s);
int  sfd_frames(const sfd *s);                  /* frames decoded so far */
/* The last decoded frame as RGBA8 (BT.601 limited range), w x h; valid
 * until the next sfd_next_frame. */
const uint8_t *sfd_rgba(sfd *s, int *w, int *h);
/* The same frame's planes as libmpeg2 gives them (for checks). */
void sfd_planes(const sfd *s, const uint8_t *p[3], int *w, int *h);
double sfd_fps(const sfd *s);

/* Audio: demux until at least `bytes` of ADX are queued (or the end). */
void sfd_audio_fill(sfd *s, int bytes);
int  sfd_audio_rate(const sfd *s);              /* 0 until the ADX header was seen */
int  sfd_audio_channels(const sfd *s);
/* Decode up to `frames` stereo frames (interleaved s16) from the queue;
 * returns the number produced (0 = nothing queued / audio over). */
int  sfd_audio_pull(sfd *s, int16_t *out, int frames);
int  sfd_audio_done(const sfd *s);

/* Decode time statistics of sfd_next_frame (milliseconds, CPU time). */
void sfd_stats(const sfd *s, int *frames, double *total_ms, double *max_ms);

#endif
