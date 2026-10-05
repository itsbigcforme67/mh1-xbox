/* audio_mix.c - portable software mixer behind audio.h. */
#include "audio.h"

#include <string.h>

typedef struct {
    int id;                 /* 0 = free */
    const int16_t *pcm;
    int n, loop;
    double pos, step;       /* in source samples */
    int rate;
    float vol, pan, pitch;
} voice;

#define RING (AUDIO_RATE)   /* one second of stereo frames */

typedef struct {
    int16_t buf[RING * 2];
    int rd, wr;             /* frames */
    int rate;
    double frac;
    float vol;
} stream;

static voice voices[AUDIO_VOICES];
static stream streams[AUDIO_STREAMS];
static int next_id = 1;

void audio_reset(void)
{
    audio_lock();
    memset(voices, 0, sizeof voices);
    memset(streams, 0, sizeof streams);
    audio_unlock();
}

static voice *find(int id)
{
    int i;
    if (id <= 0)
        return NULL;
    for (i = 0; i < AUDIO_VOICES; i++)
        if (voices[i].id == id)
            return &voices[i];
    return NULL;
}

int audio_voice_play(const int16_t *pcm, int n, int loop, int rate, float vol, float pan, float pitch)
{
    int i, best = -1;
    double best_left = 1e30;
    voice *v;

    if (!pcm || n <= 0 || rate <= 0)
        return 0;
    audio_lock();
    for (i = 0; i < AUDIO_VOICES; i++) {
        double left;
        if (!voices[i].id) {
            best = i;
            break;
        }
        /* all busy: take the one-shot voice closest to its end */
        left = voices[i].loop >= 0 ? 1e20 : (voices[i].n - voices[i].pos) / voices[i].step;
        if (left < best_left) {
            best_left = left;
            best = i;
        }
    }
    v = &voices[best];
    v->id = next_id++;
    if (next_id <= 0)
        next_id = 1;
    v->pcm = pcm;
    v->n = n;
    v->loop = loop < n ? loop : -1;
    v->pos = 0;
    v->rate = rate;
    v->vol = vol;
    v->pan = pan;
    v->pitch = pitch;
    v->step = (double)rate * pitch / AUDIO_RATE;
    i = v->id;
    audio_unlock();
    return i;
}

int audio_voice_set(int id, float vol, float pan, float pitch)
{
    voice *v;
    int ok = 0;
    audio_lock();
    v = find(id);
    if (v) {
        v->vol = vol;
        v->pan = pan;
        v->pitch = pitch;
        v->step = (double)v->rate * pitch / AUDIO_RATE;
        ok = 1;
    }
    audio_unlock();
    return ok;
}

void audio_voice_stop(int id)
{
    voice *v;
    audio_lock();
    v = find(id);
    if (v)
        v->id = 0;
    audio_unlock();
}

int audio_voice_playing(int id)
{
    int r;
    audio_lock();
    r = find(id) != NULL;
    audio_unlock();
    return r;
}

void audio_voice_stop_buffer(const int16_t *pcm, int n)
{
    int i;
    audio_lock();
    for (i = 0; i < AUDIO_VOICES; i++)
        if (voices[i].id && voices[i].pcm >= pcm && voices[i].pcm < pcm + n)
            voices[i].id = 0;
    audio_unlock();
}

int audio_stream_free(int s)
{
    int used, r;
    audio_lock();
    used = (streams[s].wr - streams[s].rd + RING) % RING;
    r = RING - 1 - used;
    audio_unlock();
    return r;
}

void audio_stream_write(int s, const int16_t *lr, int frames, int rate)
{
    stream *st = &streams[s];
    int i;
    audio_lock();
    st->rate = rate;
    for (i = 0; i < frames; i++) {
        if ((st->wr + 1) % RING == st->rd)
            break;
        st->buf[2 * st->wr] = lr[2 * i];
        st->buf[2 * st->wr + 1] = lr[2 * i + 1];
        st->wr = (st->wr + 1) % RING;
    }
    audio_unlock();
}

void audio_stream_clear(int s)
{
    audio_lock();
    streams[s].rd = streams[s].wr = 0;
    streams[s].frac = 0;
    audio_unlock();
}

void audio_stream_vol(int s, float vol)
{
    audio_lock();
    streams[s].vol = vol;
    audio_unlock();
}

/* Called with the lock held by the backend (or by the dump path). */
void audio_mix(int16_t *out, int frames)
{
    static float acc[4096 * 2];
    int i, k, done = 0;

    while (done < frames) {
        int nf = frames - done > 4096 ? 4096 : frames - done;
        memset(acc, 0, sizeof(float) * 2 * nf);
        for (k = 0; k < AUDIO_VOICES; k++) {
            voice *v = &voices[k];
            float gl, gr;
            if (!v->id)
                continue;
            gl = v->vol * (v->pan > 0 ? 1.0f - v->pan : 1.0f);
            gr = v->vol * (v->pan < 0 ? 1.0f + v->pan : 1.0f);
            for (i = 0; i < nf; i++) {
                int p = (int)v->pos;
                float f = (float)(v->pos - p), s;
                int q = p + 1;
                if (q >= v->n)
                    q = v->loop >= 0 ? v->loop : p;
                s = v->pcm[p] + (v->pcm[q] - v->pcm[p]) * f;
                acc[2 * i] += s * gl;
                acc[2 * i + 1] += s * gr;
                v->pos += v->step;
                if (v->pos >= v->n) {
                    if (v->loop < 0) {
                        v->id = 0;
                        break;
                    }
                    while (v->pos >= v->n)
                        v->pos = v->loop + (v->pos - v->n);
                }
            }
        }
        for (k = 0; k < AUDIO_STREAMS; k++) {
            stream *st = &streams[k];
            double step;
            if (st->rate <= 0)
                continue;
            step = (double)st->rate / AUDIO_RATE;
            for (i = 0; i < nf && st->rd != st->wr; i++) {
                acc[2 * i] += st->buf[2 * st->rd] * st->vol;
                acc[2 * i + 1] += st->buf[2 * st->rd + 1] * st->vol;
                st->frac += step;
                while (st->frac >= 1.0 && st->rd != st->wr) {
                    st->frac -= 1.0;
                    st->rd = (st->rd + 1) % RING;
                }
            }
        }
        for (i = 0; i < 2 * nf; i++) {
            float s = acc[i];
            out[2 * done + i] = (int16_t)(s > 32767.0f ? 32767 : s < -32768.0f ? -32768 : s);
        }
        done += nf;
    }
}
