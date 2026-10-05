/* snd.c - MH1 sound packs (SCEI HD/BD + Capcom TSBD) and CRI ADX streams.
 * Formats: docs/formats/audio.md. Host tool with the same logic:
 * tools/snd_dump.py. */
#include "fmt.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

#define U32(p) fmt_u32((p), FMT_LE)
#define U16(p) fmt_u16((p), FMT_LE)

/* An SCEI chunk: "SCEI"+tag stored as two LE words, u32 size, u32 max
 * index, then max+1 u32 offsets from the chunk start (0xFFFFFFFF = none). */
static const uint8_t *chunk(const uint8_t *hd, uint32_t off, const char *magic, int *n)
{
    const uint8_t *c = hd + off;
    if (memcmp(c, magic, 8) != 0)
        return NULL;
    *n = (int)U32(c + 12) + 1;
    return c;
}

static const uint8_t *entry(const uint8_t *c, int n, int i)
{
    uint32_t o;
    if (!c || i < 0 || i >= n)
        return NULL;
    o = U32(c + 16 + 4 * i);
    return o == 0xFFFFFFFFu ? NULL : c + o;
}

int fmt_snd_open(snd_pack *p, const uint8_t *d, size_t n)
{
    uint32_t ns, hd_off, bd_off;
    const uint8_t *head;

    memset(p, 0, sizeof *p);
    if (n < 0x40 || memcmp(d, "MOMO", 4) != 0)
        return -1;
    ns = U32(d + 4);
    if (ns < 2)
        return -1;
    hd_off = U32(d + 8);
    bd_off = U32(d + 16);
    if (hd_off + 0x50 > n || bd_off > n)
        return -1;
    p->hd = d + hd_off;
    p->bd = d + bd_off;
    head = p->hd + 0x10;                       /* SCEIHead */
    if (memcmp(head, "IECSdaeH", 8) != 0)
        return -1;
    p->bd_size = U32(head + 0x10);
    p->prog = chunk(p->hd, U32(head + 0x14), "IECSgorP", &p->nprog);
    p->sset = chunk(p->hd, U32(head + 0x18), "IECStesS", &p->nsset);
    p->smpl = chunk(p->hd, U32(head + 0x1C), "IECSlpmS", &p->nsmpl);
    p->vagi = chunk(p->hd, U32(head + 0x20), "IECSigaV", &p->nvagi);
    if (!p->prog || !p->sset || !p->smpl || !p->vagi)
        return -1;
    if (ns >= 4) {
        uint32_t t = U32(d + 32), sz = U32(d + 36);
        if (t + sz <= n && memcmp(d + t, "TSBD", 4) == 0) {
            p->tsbd = d + t + 16;
            p->ntsbd = (int)((sz - 16) / 16);
        }
    }
    return 0;
}

const uint8_t *fmt_snd_tsbd(const snd_pack *p, int code)
{
    const uint8_t *e;
    if (!p->tsbd || code < 0 || code >= p->ntsbd)
        return NULL;
    e = p->tsbd + 16 * code;
    return e[0] == 0xFF ? NULL : e;
}

int fmt_snd_has_prog(const snd_pack *p, int prog)
{
    return entry(p->prog, p->nprog, prog) != NULL;
}

/* Program param (0x24 bytes): u32 split block offset, u8 nsplit, u8 split
 * size, u8 volume, u8 pan, ... Split block (0x14): u16 sample set,
 * u8 low note, u8 crossfade, u8 high note, u8 number, u16 bend range low,
 * u16 bend range high (0x0200 = 2 semitones [inferred]), ..., +0x10
 * volume, +0x11 pan, +0x12 transpose, +0x13 detune (the SDK's
 * SceHdSplitBlock order). Sample set: u8 vel curve, vel low, vel high,
 * u8 count, u16 samples[]. Sample (0x2A): u16 VAG index, ..., +0x0B base
 * note, +0x0C detune, +0x0D pan, +0x10 volume. */
int fmt_snd_resolve(const snd_pack *p, int prog, int note, snd_note *out)
{
    const uint8_t *pg = entry(p->prog, p->nprog, prog);
    int k, ns, sz;

    if (!pg)
        return -1;
    ns = pg[4];
    sz = pg[5];
    for (k = 0; k < ns; k++) {
        const uint8_t *sp = pg + U32(pg) + k * sz;
        const uint8_t *set, *smp;
        float semis;
        if (note < sp[2] || note > sp[4])
            continue;
        set = entry(p->sset, p->nsset, U16(sp));
        if (!set || set[3] == 0)
            return -1;
        smp = entry(p->smpl, p->nsmpl, U16(set + 4));
        if (!smp)
            return -1;
        semis = (float)(note + (int8_t)sp[18] - smp[11]) + (int8_t)smp[12] / 128.0f + (int8_t)sp[19] / 128.0f;
        out->vag = U16(smp);
        out->ratio = powf(2.0f, semis / 12.0f);
        out->vol = (pg[6] / 127.0f) * (sp[16] / 127.0f) * (smp[16] / 127.0f);
        out->bend_lo = sp[7];
        out->bend_hi = sp[9];
        out->pan = ((int)pg[7] + sp[17] + smp[13] - 3 * 64) / 64.0f;
        if (out->pan < -1.0f) out->pan = -1.0f;
        if (out->pan > 1.0f) out->pan = 1.0f;
        return 0;
    }
    return -1;
}

int fmt_snd_vag(const snd_pack *p, int vag, uint32_t *off, int *rate)
{
    const uint8_t *v = entry(p->vagi, p->nvagi, vag);
    if (!v)
        return -1;
    *off = U32(v);
    *rate = U16(v + 4);
    return *off < p->bd_size ? 0 : -1;
}

static const int vag_coef[5][2] = { {0, 0}, {60, 0}, {115, -52}, {98, -55}, {122, -60} };

int16_t *fmt_vag_decode(const uint8_t *src, size_t max, int *nsamples, int *loop)
{
    size_t frames = max / 16, f;
    int16_t *out = malloc((frames ? frames : 1) * 28 * sizeof *out);
    int h1 = 0, h2 = 0, n = 0, k;

    *loop = -1;
    for (f = 0; f < frames; f++) {
        const uint8_t *q = src + 16 * f;
        int shift = q[0] & 0xF, filt = q[0] >> 4, flags = q[1];
        int c0 = filt < 5 ? vag_coef[filt][0] : 0, c1 = filt < 5 ? vag_coef[filt][1] : 0;
        if (flags & 4)
            *loop = n;
        for (k = 0; k < 28; k++) {
            int nib = (q[2 + k / 2] >> ((k & 1) * 4)) & 0xF;
            int s = (int16_t)(nib << 12);
            s = (s >> shift) + ((h1 * c0 + h2 * c1) >> 6);
            if (s > 32767) s = 32767;
            if (s < -32768) s = -32768;
            out[n++] = (int16_t)s;
            h2 = h1;
            h1 = s;
        }
        if (flags & 1) {                    /* end; with bit 2 it jumps to the loop start */
            if (!(flags & 2))
                *loop = -1;
            break;
        }
    }
    *nsamples = n;
    return out;
}

/* ------------------------------------------------------------ ADX */
static uint32_t be32(const uint8_t *p) { return fmt_u32(p, FMT_BE); }

int fmt_adx_header(adx_info *h, const uint8_t *p, size_t n)
{
    int ver, cutoff;
    uint32_t base;
    double z, a, b, c;

    memset(h, 0, sizeof *h);
    if (n < 0x20 || p[0] != 0x80 || p[4] != 3 || p[6] != 4)
        return -1;
    h->data = fmt_u16(p + 2, FMT_BE) + 4u;
    h->block = p[5];
    h->ch = p[7];
    h->rate = (int)be32(p + 8);
    h->total = be32(p + 12);
    cutoff = fmt_u16(p + 16, FMT_BE);
    ver = p[18];
    if (p[19] != 0 || h->ch < 1 || h->ch > 2 || h->block != 18)
        return -1;                          /* encrypted or unusual: not on the MH1 disc */
    base = ver == 3 ? 0x14 : 0x20;
    if (h->data >= base + 0x18 && n >= base + 0x18 && be32(p + base + 4) == 1) {
        h->loop = 1;
        h->loop_start = be32(p + base + 8);
        h->loop_start_byte = be32(p + base + 12);
        h->loop_end = be32(p + base + 16);
        h->loop_end_byte = be32(p + base + 20);
    }
    z = cos(2.0 * M_PI * cutoff / h->rate);
    a = M_SQRT2 - z;
    b = M_SQRT2 - 1.0;
    c = (a - sqrt((a + b) * (a - b))) / b;
    h->c1 = (int)(c * 8192.0);
    h->c2 = (int)(c * c * -4096.0);
    return 0;
}

void fmt_adx_row(const adx_info *h, const uint8_t *in, int32_t (*hist)[2], int16_t *out)
{
    int c, k;
    for (c = 0; c < h->ch; c++) {
        const uint8_t *fr = in + c * h->block;
        int scale = fmt_u16(fr, FMT_BE) + 1;
        int32_t h1 = hist[c][0], h2 = hist[c][1];
        for (k = 0; k < 32; k++) {
            int nib = (fr[2 + k / 2] >> ((k & 1) ? 0 : 4)) & 0xF;
            int32_t s = (nib & 8 ? nib - 16 : nib) * scale + ((h->c1 * h1 + h->c2 * h2) >> 12);
            if (s > 32767) s = 32767;
            if (s < -32768) s = -32768;
            out[k * h->ch + c] = (int16_t)s;
            h2 = h1;
            h1 = s;
        }
        hist[c][0] = h1;
        hist[c][1] = h2;
    }
}
