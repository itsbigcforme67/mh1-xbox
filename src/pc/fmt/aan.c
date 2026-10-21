/*
 * aan.c - motion tables (*_tbl.bin) and AAN motions (docs/formats/motion.md).
 *
 * tbl: {u32 slot count, u32 offset} per bank until a count of 0; each bank
 * is slot count x s32 AAN offsets from the file start (-1 = empty).
 * AAN: header {u32 kind | 0x80000000, u32 bones, u32 size, u32 loop,
 * f32 loop start}; per bone {u32 0x80000000 | channel mask, u32 curves,
 * u32 size}; per curve {u32 0x80000000 | fmt << 16 | 1 << channel,
 * u32 keys, u32 size} + keys. Evaluation follows flFCVGetValue2 (0x170240)
 * and flFCVFcurveInterpolateHermite (0x171160).
 */
#include "fmt.h"

#include <stdlib.h>
#include <string.h>

static int keysize(int fmt)
{
    switch (fmt) {
    case 0x21: return 8;
    case 0x22: return 16;
    case 0x23: return 20;
    case 0x11: return 4;
    case 0x12: return 8;
    case 0x13: return 12;
    }
    return 0;
}

fmt_blob fmt_tbl_motion(fmt_blob tbl, int bank, int slot, int be)
{
    fmt_blob r = { NULL, 0 };
    int b;
    uint32_t cnt, off;
    int32_t mo;
    for (b = 0; b <= bank; b++) {
        if (8 * (size_t)b + 8 > tbl.n || fmt_u32(tbl.p + 8 * b, be) == 0)
            return r;
    }
    cnt = fmt_u32(tbl.p + 8 * bank, be);
    off = fmt_u32(tbl.p + 8 * bank + 4, be);
    if (slot < 0 || (uint32_t)slot >= cnt || off + 4 * (size_t)slot + 4 > tbl.n)
        return r;
    mo = fmt_s32(tbl.p + off + 4 * slot, be);
    if (mo < 0 || (size_t)mo + 0x14 > tbl.n)
        return r;
    r.p = tbl.p + mo;
    r.n = fmt_u32(r.p + 8, be);
    if ((size_t)mo + r.n > tbl.n)
        r.p = NULL, r.n = 0;
    return r;
}

/* key fields: value, time, in-slope, out-slope (as floats) */
static void key_get(const aan_curve *c, int i, int be, float k[4], int *interp)
{
    const uint8_t *p = c->keys + (size_t)i * keysize(c->format);
    *interp = 0x20000;
    switch (c->format) {
    case 0x21: k[0] = fmt_f32(p, be); k[1] = fmt_f32(p + 4, be); k[2] = k[3] = 0; *interp = 0x10000; break;
    case 0x22: k[0] = fmt_f32(p, be); k[1] = fmt_f32(p + 4, be); k[2] = fmt_f32(p + 8, be); k[3] = fmt_f32(p + 12, be); break;
    case 0x23: *interp = fmt_s32(p, be);
               k[0] = fmt_f32(p + 4, be); k[1] = fmt_f32(p + 8, be); k[2] = fmt_f32(p + 12, be); k[3] = fmt_f32(p + 16, be); break;
    case 0x11: k[0] = fmt_s16(p, be); k[1] = fmt_s16(p + 2, be); k[2] = k[3] = 0; *interp = 0x10000; break;
    case 0x12: k[0] = fmt_s16(p, be); k[1] = fmt_s16(p + 2, be); k[2] = fmt_s16(p + 4, be); k[3] = fmt_s16(p + 6, be); break;
    case 0x13: *interp = fmt_s32(p, be);
               k[0] = fmt_s16(p + 4, be); k[1] = fmt_s16(p + 6, be); k[2] = fmt_s16(p + 8, be); k[3] = fmt_s16(p + 10, be); break;
    default: k[0] = k[1] = k[2] = k[3] = 0;
    }
}

int fmt_aan_load(aan_motion *m, fmt_blob f, int be)
{
    size_t o;
    int b;

    memset(m, 0, sizeof *m);
    if (f.n < 0x14)
        return -1;
    m->be = be;
    m->kind = (int)(fmt_u32(f.p, be) & 0xFF);
    m->nbone = (int)fmt_u32(f.p + 4, be);
    m->loop = (int)fmt_u32(f.p + 0xC, be);
    m->loop_start = fmt_f32(f.p + 0x10, be);
    m->ncurve = calloc(m->nbone + 1, sizeof(int));
    m->curve = calloc(m->nbone + 1, sizeof(aan_curve *));
    o = 0x14;
    for (b = 0; b < m->nbone && o + 12 <= f.n; b++) {
        uint32_t nc = fmt_u32(f.p + o + 4, be), bs = fmt_u32(f.p + o + 8, be), c;
        size_t q = o + 12;
        m->curve[b] = calloc(nc + 1, sizeof(aan_curve));
        for (c = 0; c < nc && q + 12 <= f.n; c++) {
            uint32_t ct = fmt_u32(f.p + q, be), cs = fmt_u32(f.p + q + 8, be);
            aan_curve *cv = &m->curve[b][m->ncurve[b]];
            unsigned mask = ct & 0x1FF;
            int ch = 0;
            while (mask > 1) {
                mask >>= 1;
                ch++;
            }
            cv->channel = ch;
            cv->format = (int)((ct >> 16) & 0xFF);
            cv->nkey = (int)fmt_u32(f.p + q + 4, be);
            cv->keys = f.p + q + 12;
            if (keysize(cv->format) && cv->nkey > 0) {
                float k[4];
                int ip;
                key_get(cv, cv->nkey - 1, be, k, &ip);
                if (k[1] > m->end)
                    m->end = k[1];
                m->ncurve[b]++;
            }
            q += cs;
        }
        o += bs < 12 ? 12 : bs;
    }
    return 0;
}

void fmt_aan_free(aan_motion *m)
{
    int b;
    for (b = 0; b < m->nbone; b++)
        free(m->curve[b]);
    free(m->curve);
    free(m->ncurve);
    memset(m, 0, sizeof *m);
}

/* flFCVFcurveInterpolateLinear / flFCVFcurveInterpolateHermite (main 0x1710E0 / 0x171160, VU0 code [read: asm]):
 * the PS2's own formulas, operation for operation, so the game's motions (and with them the root motion that moves
 * hunters and monsters) come out as on the PS2 under its float rounding (src/pc/rt/rt_ref.c). Until 20 Oct 2026 this
 * used the textbook u = (t - t0) / (t1 - t0) forms, equal in maths, a few bits off in floats (the lockstep referee). */
static float interp_linear(float t, float v0, float t0, float v1, float t1)
{
    float q = 1.0f / (t1 - t0);
    float slope = (v1 - v0) * q, base = (t1 * v0 - t0 * v1) * q;
    return slope * t + base;
}
static float interp_hermite(float t, float v0, float t0, float s0, float v1, float t1, float s1)
{
    float d = t - t0, r = 1.0f / (t1 - t0);
    float r2 = r * r, d2 = d * d, q = d2 * r, c = (d2 * d) * r2;
    float h2 = (2.0f * c) * r, h3 = (3.0f * d2) * r2;
    float acc = v0 * ((h2 - h3) + 1.0f);
    acc = acc + v1 * (h3 - h2);
    acc = acc + s0 * (((c - q) - q) + d);
    acc = acc + s1 * (c - q);
    return acc;
}

/* flFCVGetValue2 (0x170240) with its key search (flFCVSeekFcurveKey*): at or before the first key, at or after the
 * last one, or exactly on a key, that key's value; else the segment around t. A complex key (formats 0x13 / 0x23)
 * names its segment's interpolation: 0x10000 linear, 0x20000 Hermite, any other value gives 0. */
static float curve_value_vu(const aan_curve *c, float t, int be)
{
    float a[4], b[4];
    int ia, ib, lo, hi;
    key_get(c, 0, be, a, &ia);
    if (t <= a[1] || c->nkey == 1)
        return a[0];
    key_get(c, c->nkey - 1, be, b, &ib);
    if (!(t < b[1]))
        return b[0];
    lo = 0;
    hi = c->nkey - 1;                 /* binary search for k[lo].t <= t < k[hi].t */
    while (hi - lo > 1) {
        int mid = (lo + hi) / 2;
        key_get(c, mid, be, a, &ia);
        if (a[1] <= t)
            lo = mid;
        else
            hi = mid;
    }
    key_get(c, lo, be, a, &ia);
    if (a[1] == t)
        return a[0];
    key_get(c, hi, be, b, &ib);
    if (ia == 0x10000)
        return interp_linear(t, a[0], a[1], b[0], b[1]);
    if (ia != 0x20000)
        return 0.0f;
    return interp_hermite(t, a[0], a[1], a[3], b[0], b[1], b[2]);
}

#include "../rt/rt_vu.h"
static float curve_value(const aan_curve *c, float t, int be)
{
    unsigned int o = rt_vu_begin();     /* VU0 code on the PS2 (rt_vu.h) */
    float v = curve_value_vu(c, t, be);
    rt_vu_end(o);
    return v;
}

void fmt_aan_eval(const aan_motion *m, int bone, float t, float chan[9])
{
    int c;
    if (bone < 0 || bone >= m->nbone)
        return;
    for (c = 0; c < m->ncurve[bone]; c++) {
        const aan_curve *cv = &m->curve[bone][c];
        float v = curve_value(cv, t, m->be);
        if (cv->channel < 0 || cv->channel > 8)
            continue;
        if (m->kind == 2)   /* short motions: rotation 2*pi/16384 per unit, rest /16 */
            v = (cv->channel >= 3 && cv->channel <= 5) ? v * 0.0003834952f : v / 16.0f;
        chan[cv->channel] = v;
    }
}
