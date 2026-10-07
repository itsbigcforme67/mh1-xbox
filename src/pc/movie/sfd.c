/* sfd.c - see sfd.h. */
#include "sfd.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

/* mpeg2.h declares a member named malloc: leave out the port's malloc counting macros (rt_memstat.h) */
#undef malloc
#undef calloc
#undef realloc
#undef free
#include "../../../third_party/libmpeg2/mpeg2.h"

#define SECTOR 2048

typedef struct { uint8_t *p; size_t rd, wr, cap; } bq;       /* byte queue */

struct sfd {
    const fmt_afs *afs;
    int idx;
    uint32_t pos, size;             /* next sector offset / entry size */
    int eof;
    bq vq, aq;                      /* video ES bytes, audio ES bytes */
    uint8_t *chunk;                 /* what libmpeg2 is parsing (the queues may move) */
    size_t chunk_cap;
    mpeg2dec_t *dec;
    const mpeg2_info_t *info;
    int seq_ok, ended, end_sent;
    int nframes;
    const uint8_t *pl[3];
    int w, h, cw;
    double fps;
    uint8_t *rgba;
    int rgba_cap;
    /* audio */
    adx_info ah;
    int ah_ok;
    uint32_t a_samples;             /* samples (per channel) decoded */
    int32_t hist[2][2];
    uint32_t a_skip;                /* header bytes still to drop */
    int a_done;
    /* stats */
    int st_n;
    double st_total, st_max;
};

static double cpu_ms(void) { return (double)clock() * 1000.0 / CLOCKS_PER_SEC; }   /* CPU time: portable C */

/* ------------------------------------------------------------ byte queue */
static void bq_add(bq *q, const uint8_t *d, size_t n)
{
    if (q->rd == q->wr)
        q->rd = q->wr = 0;
    else if (q->wr + n > q->cap && q->rd > 0) {
        memmove(q->p, q->p + q->rd, q->wr - q->rd);
        q->wr -= q->rd;
        q->rd = 0;
    }
    if (q->wr + n > q->cap) {
        q->cap = (q->wr + n) * 2 + 65536;
        q->p = realloc(q->p, q->cap);
    }
    memcpy(q->p + q->wr, d, n);
    q->wr += n;
}

/* ------------------------------------------------------------ demux */
/* One 2048-byte sector: pack header, optional system header, then PES
 * packets (MPEG-1 style: stuffing, optional STD buffer, PTS/DTS). */
static void demux_sector(sfd *s, const uint8_t *d, int n)
{
    int i = 0;
    while (i + 6 <= n) {
        int c, len, j, end;
        if (d[i] || d[i + 1] || d[i + 2] != 1) {
            i++;
            continue;
        }
        c = d[i + 3];
        if (c == 0xBA) {                 /* pack header: 12 bytes (MPEG-1) or 14 + stuffing (MPEG-2) */
            i += (d[i + 4] >> 6) == 1 ? 14 + (d[i + 13] & 7) : 12;
            continue;
        }
        if (c < 0xBB) {
            i += 4;
            continue;
        }
        len = (d[i + 4] << 8) | d[i + 5];
        end = i + 6 + len;
        if (end > n)
            end = n;
        if (c == 0xE0 || c == 0xC0) {
            j = i + 6;
            if ((d[j] >> 6) == 2) {          /* MPEG-2 PES header */
                j += 3 + d[j + 2];
            } else {                         /* MPEG-1 */
                while (j < end && d[j] == 0xFF)
                    j++;
                if (j < end && (d[j] >> 6) == 1)
                    j += 2;
                if (j < end && (d[j] >> 4) == 2)
                    j += 5;
                else if (j < end && (d[j] >> 4) == 3)
                    j += 10;
                else
                    j++;
            }
            if (j < end)
                bq_add(c == 0xE0 ? &s->vq : &s->aq, d + j, (size_t)(end - j));
        }
        i = end;
    }
}

static void read_sector(sfd *s)
{
    uint8_t buf[SECTOR];
    size_t got;
    if (s->eof)
        return;
    got = fmt_afs_read_at(s->afs, s->idx, s->pos, buf, SECTOR);
    if (got == 0) {
        s->eof = 1;
        return;
    }
    s->pos += SECTOR;
    if (s->pos >= s->size)
        s->eof = 1;
    demux_sector(s, buf, (int)got);
}

/* ------------------------------------------------------------ audio */
static void audio_header(sfd *s)
{
    size_t have = s->aq.wr - s->aq.rd;
    if (s->ah_ok || have < 0x40)
        return;
    if (fmt_adx_header(&s->ah, s->aq.p + s->aq.rd, have) != 0) {
        s->a_done = 1;
        s->aq.rd = s->aq.wr = 0;
        return;
    }
    if (have < s->ah.data)
        return;                         /* header not complete yet */
    s->ah_ok = 1;
    s->aq.rd += s->ah.data;
    s->a_samples = 0;
}

void sfd_audio_fill(sfd *s, int bytes)
{
    while (!s->eof && (int)(s->aq.wr - s->aq.rd) < bytes)
        read_sector(s);
    audio_header(s);
}

int sfd_audio_rate(const sfd *s) { return s->ah_ok ? s->ah.rate : 0; }
int sfd_audio_channels(const sfd *s) { return s->ah_ok ? s->ah.ch : 0; }
int sfd_audio_done(const sfd *s) { return s->a_done; }

int sfd_audio_pull(sfd *s, int16_t *out, int frames)
{
    int n = 0;
    audio_header(s);
    if (!s->ah_ok || s->a_done)
        return 0;
    while (n + 32 <= frames) {
        int16_t tmp[64];
        int k, take = 32;
        size_t row = (size_t)(s->ah.ch * s->ah.block);
        if (s->aq.wr - s->aq.rd < row)
            break;
        if (s->a_samples >= s->ah.total) {
            s->a_done = 1;
            break;
        }
        fmt_adx_row(&s->ah, s->aq.p + s->aq.rd, s->hist, tmp);
        s->aq.rd += row;
        if (s->a_samples + 32 > s->ah.total)
            take = (int)(s->ah.total - s->a_samples);
        for (k = 0; k < take; k++) {
            out[2 * (n + k)] = tmp[k * s->ah.ch];
            out[2 * (n + k) + 1] = tmp[k * s->ah.ch + s->ah.ch - 1];
        }
        n += take;
        s->a_samples += 32;
    }
    return n;
}

/* ------------------------------------------------------------ video */
sfd *sfd_open(const fmt_afs *afs, int idx)
{
    sfd *s;
    uint8_t head[SECTOR];
    if (idx < 0 || idx >= (int)afs->count)
        return NULL;
    if (fmt_afs_read_at(afs, idx, 0, head, SECTOR) < SECTOR || head[0] || head[1] || head[2] != 1 || head[3] != 0xBA)
        return NULL;
    s = calloc(1, sizeof *s);
    s->afs = afs;
    s->idx = idx;
    s->size = afs->size[idx];
    s->dec = mpeg2_init();
    if (!s->dec) {
        free(s);
        return NULL;
    }
    s->info = mpeg2_info(s->dec);
    s->fps = 29.97;
    return s;
}

void sfd_close(sfd *s)
{
    if (!s)
        return;
    mpeg2_close(s->dec);
    free(s->vq.p);
    free(s->chunk);
    free(s->aq.p);
    free(s->rgba);
    free(s);
}

int sfd_frames(const sfd *s) { return s->nframes; }
double sfd_fps(const sfd *s) { return s->fps; }

void sfd_stats(const sfd *s, int *frames, double *total_ms, double *max_ms)
{
    *frames = s->st_n;
    *total_ms = s->st_total;
    *max_ms = s->st_max;
}

/* Hand libmpeg2 the next piece of video ES (reading sectors as needed).
 * Compaction of the queue happens only here, i.e. when libmpeg2 has
 * finished with the previous piece. */
static int feed(sfd *s)
{
    static uint8_t endcode[4] = { 0, 0, 1, 0xB7 };
    while (s->vq.rd == s->vq.wr && !s->eof)
        read_sector(s);
    if (s->vq.rd == s->vq.wr) {
        if (s->end_sent)
            return 0;
        s->end_sent = 1;
        mpeg2_buffer(s->dec, endcode, endcode + 4);
        return 1;
    }
    {
        size_t n = s->vq.wr - s->vq.rd;
        if (n > s->chunk_cap) {
            s->chunk_cap = n * 2;
            s->chunk = realloc(s->chunk, s->chunk_cap);
        }
        memcpy(s->chunk, s->vq.p + s->vq.rd, n);
        mpeg2_buffer(s->dec, s->chunk, s->chunk + n);
        s->vq.rd = s->vq.wr;
    }
    return 1;
}

int sfd_next_frame(sfd *s)
{
    double t0 = cpu_ms(), dt;
    int got = 0;
    if (s->ended)
        return 0;
    for (;;) {
        mpeg2_state_t st = mpeg2_parse(s->dec);
        switch (st) {
        case STATE_BUFFER:
            if (!feed(s)) {
                s->ended = 1;
                return 0;
            }
            break;
        case STATE_SEQUENCE:
            s->w = (int)s->info->sequence->width;
            s->h = (int)s->info->sequence->height;
            s->cw = (int)s->info->sequence->chroma_width;
            if (s->info->sequence->frame_period)
                s->fps = 27000000.0 / s->info->sequence->frame_period;
            break;
        case STATE_SLICE:
        case STATE_END:
        case STATE_INVALID_END:
            if (s->info->display_fbuf) {
                s->pl[0] = s->info->display_fbuf->buf[0];
                s->pl[1] = s->info->display_fbuf->buf[1];
                s->pl[2] = s->info->display_fbuf->buf[2];
                s->nframes++;
                got = 1;
            }
            if (st != STATE_SLICE && !got) {
                s->ended = 1;
                return 0;
            }
            break;
        default:
            break;
        }
        if (got)
            break;
    }
    dt = cpu_ms() - t0;
    s->st_n++;
    s->st_total += dt;
    if (dt > s->st_max)
        s->st_max = dt;
    return 1;
}

void sfd_planes(const sfd *s, const uint8_t *p[3], int *w, int *h)
{
    p[0] = s->pl[0];
    p[1] = s->pl[1];
    p[2] = s->pl[2];
    *w = s->w;
    *h = s->h;
}

static uint8_t clamp8(int v) { return (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v); }

/* BT.601, studio range, 4:2:0 -> RGBA */
const uint8_t *sfd_rgba(sfd *s, int *w, int *h)
{
    int x, y, cw = s->cw;
    if (!s->pl[0])
        return NULL;
    if (s->rgba_cap < s->w * s->h * 4) {
        s->rgba_cap = s->w * s->h * 4;
        s->rgba = realloc(s->rgba, (size_t)s->rgba_cap);
    }
    for (y = 0; y < s->h; y++) {
        const uint8_t *py = s->pl[0] + y * s->w;
        const uint8_t *pu = s->pl[1] + (y >> 1) * cw;
        const uint8_t *pv = s->pl[2] + (y >> 1) * cw;
        uint8_t *o = s->rgba + (size_t)y * s->w * 4;
        for (x = 0; x < s->w; x++) {
            int c = (py[x] - 16) * 298, d = pu[x >> 1] - 128, e = pv[x >> 1] - 128;
            o[4 * x] = clamp8((c + 409 * e + 128) >> 8);
            o[4 * x + 1] = clamp8((c - 100 * d - 208 * e + 128) >> 8);
            o[4 * x + 2] = clamp8((c + 516 * d + 128) >> 8);
            o[4 * x + 3] = 255;
        }
    }
    *w = s->w;
    *h = s->h;
    return s->rgba;
}
