/*
 * lzari.c - LZARI, the compression of PS2 ".max" (Action Replay Max) saves: an LZSS dictionary
 * coder (4096 byte window, matches of 3..60 bytes) whose literals, match lengths and match
 * positions are coded by an adaptive arithmetic coder (Haruhiko Okumura's LZARI, 1988). Written
 * here from the algorithm as documented in mymc's public domain lzari.py (see
 * docs/formats/saves.md); the stream has no length prefix (the MAX header carries the size).
 */
#include <stdlib.h>
#include <string.h>
#include "ps2save.h"

#define HIST 4096
#define MINM 3
#define MAXM 60
#define QB 15
#define Q1 (1u << QB)
#define Q2 (Q1 * 2)
#define Q3 (Q1 * 3)
#define Q4 (Q1 * 4)
#define MAX_CUM (Q1 - 1)
#define NCHAR (256 + MAXM - MINM + 1)   /* 314 symbols: 256 literals, 58 match lengths */

typedef struct {
    unsigned sym_freq[NCHAR + 1], sym_cum[NCHAR + 1], pos_cum[HIST + 1];
    int sym_to_char[NCHAR + 1], char_to_sym[NCHAR];
} Model;

static void model_init(Model *m)
{
    int i;
    unsigned a = 0;
    m->sym_freq[0] = 0;
    m->sym_to_char[0] = 0;
    for (i = 1; i <= NCHAR; i++) {
        m->sym_freq[i] = 1;
        m->sym_to_char[i] = i - 1;
        m->char_to_sym[i - 1] = i;
    }
    for (i = 0; i <= NCHAR; i++)
        m->sym_cum[i] = (unsigned)(NCHAR - i);
    for (i = HIST; i > 0; i--) {
        a += 10000u / (200u + (unsigned)i);
        m->pos_cum[i - 1] = a;
    }
    m->pos_cum[HIST] = 0;
}

static void model_update(Model *m, int sym)
{
    int i, ns;
    unsigned freq;
    if (m->sym_cum[0] >= MAX_CUM) {
        unsigned c = 0;
        for (i = NCHAR; i > 0; i--) {
            m->sym_cum[i] = c;
            m->sym_freq[i] = (m->sym_freq[i] + 1) >> 1;
            c += m->sym_freq[i];
        }
        m->sym_cum[0] = c;
    }
    freq = m->sym_freq[sym];
    ns = sym;
    while (m->sym_freq[ns - 1] == freq)
        ns--;
    if (ns != sym) {
        int sc = m->sym_to_char[ns], ch = m->sym_to_char[sym];
        m->sym_to_char[ns] = ch;
        m->sym_to_char[sym] = sc;
        m->char_to_sym[ch] = ns;
        m->char_to_sym[sc] = sym;
    }
    m->sym_freq[ns]++;
    for (i = 0; i < ns; i++)
        m->sym_cum[i]++;
}

/* ------------------------------------------------------------------ encoder */
typedef struct {
    uint8_t *buf;
    size_t len, cap;
    int bitn;           /* bits in the byte being filled */
    unsigned cur;
    unsigned low, high, shifts;
    int oom;
} Enc;

static void put_bit_raw(Enc *e, int b)
{
    e->cur = e->cur << 1 | (unsigned)b;
    if (++e->bitn == 8) {
        if (e->len == e->cap) {
            uint8_t *nb = realloc(e->buf, e->cap ? e->cap * 2 : 4096);
            if (!nb) {
                e->oom = 1;
                return;
            }
            e->buf = nb;
            e->cap = e->cap ? e->cap * 2 : 4096;
        }
        e->buf[e->len++] = (uint8_t)e->cur;
        e->bitn = 0;
        e->cur = 0;
    }
}

static void put_bit(Enc *e, int b)
{
    put_bit_raw(e, b);
    for (; e->shifts; e->shifts--)
        put_bit_raw(e, !b);
}

/* narrow [low,high) to [cum_low.., cum_high..) of total, then renormalise */
static void enc_range(Enc *e, unsigned cum_hi, unsigned cum_lo, unsigned total)
{
    unsigned range = e->high - e->low;
    unsigned high = e->low + (unsigned)((uint64_t)range * cum_hi / total);
    unsigned low = e->low + (unsigned)((uint64_t)range * cum_lo / total);
    for (;;) {
        if (high <= Q2)
            put_bit(e, 0);
        else if (low >= Q2) {
            put_bit(e, 1);
            low -= Q2;
            high -= Q2;
        } else if (low >= Q1 && high <= Q3) {
            e->shifts++;
            low -= Q1;
            high -= Q1;
        } else
            break;
        low *= 2;
        high *= 2;
    }
    e->low = low;
    e->high = high;
}

static void enc_char(Enc *e, Model *m, int ch)
{
    int sym = m->char_to_sym[ch];
    enc_range(e, m->sym_cum[sym - 1], m->sym_cum[sym], m->sym_cum[0]);
    model_update(m, sym);
}

static void enc_pos(Enc *e, Model *m, int pos)
{
    enc_range(e, m->pos_cum[pos], m->pos_cum[pos + 1], m->pos_cum[0]);
}

uint8_t *lzari_encode(const uint8_t *src, size_t n, size_t *outn)
{
    Enc e;
    Model *m;
    uint8_t *s;
    int *head, *prev;
    size_t i, pad, total;
    memset(&e, 0, sizeof e);
    *outn = 0;
    if (n == 0)
        return NULL;
    pad = n < MAXM ? n : MAXM;                  /* the stream starts with `pad` spaces of history */
    total = pad + n;
    m = malloc(sizeof *m);
    s = malloc(total);
    head = malloc(65536 * sizeof *head);
    prev = malloc(total * sizeof *prev);
    if (!m || !s || !head || !prev) {
        free(m); free(s); free(head); free(prev);
        return NULL;
    }
    memset(s, ' ', pad);
    memcpy(s + pad, src, n);
    for (i = 0; i < 65536; i++)
        head[i] = -1;
    model_init(m);
    e.low = 0;
    e.high = Q4;

#define HASH(p) (((unsigned)s[p] << 8 ^ (unsigned)s[(p) + 1] << 4 ^ (unsigned)s[(p) + 2]) & 0xFFFF)
    {
        size_t pos = 0;
        size_t maxm = pad;
        /* index the padding first so matches may reach into it */
        while (pos < total) {
            int best = 0, bestpos = 0;
            size_t lim = total - pos < maxm ? total - pos : maxm;
            if (pos >= pad && lim >= MINM) {
                int p = head[HASH(pos)], tries = 128;
                while (p >= 0 && tries-- > 0 && pos - (size_t)p <= HIST) {
                    size_t l = 0;
                    while (l < lim && s[(size_t)p + l] == s[pos + l])
                        l++;
                    if ((int)l > best) {
                        best = (int)l;
                        bestpos = p;
                        if (l == lim)
                            break;
                    }
                    p = prev[p];
                }
            }
            if (pos >= pad) {
                if (best >= MINM) {
                    enc_char(&e, m, 256 - MINM + best);
                    enc_pos(&e, m, (int)(pos - (size_t)bestpos) - 1);
                } else
                    enc_char(&e, m, s[pos]);
            }
            /* index the bytes just consumed (including the pad) */
            {
                size_t step = (pos >= pad && best >= MINM) ? (size_t)best : 1, k;
                for (k = 0; k < step; k++, pos++) {
                    if (pos + 2 < total) {
                        unsigned h = HASH(pos);
                        prev[pos] = head[h];
                        head[h] = (int)pos;
                    } else
                        prev[pos] = -1;
                }
            }
        }
    }
#undef HASH
    e.shifts++;
    put_bit(&e, e.low < Q1 ? 0 : 1);
    while (e.bitn)
        put_bit_raw(&e, 0);
    free(m);
    free(s);
    free(head);
    free(prev);
    if (e.oom) {
        free(e.buf);
        return NULL;
    }
    *outn = e.len;
    return e.buf;
}

/* ------------------------------------------------------------------ decoder */
typedef struct {
    const uint8_t *p;
    size_t n, at;
    unsigned bit;       /* bits left in the current byte */
    unsigned cur;
    unsigned low, high, code;
} Dec;

static unsigned get_bit(Dec *d)
{
    unsigned b;
    if (d->bit == 0) {
        d->cur = d->at < d->n ? d->p[d->at] : 0;    /* past the end: zero bits */
        d->at++;
        d->bit = 8;
    }
    d->bit--;
    b = d->cur >> d->bit & 1;
    return b;
}

/* after choosing [low,high): renormalise and read in new bits */
static void dec_norm(Dec *d)
{
    for (;;) {
        if (d->low < Q2) {
            if (d->low < Q1 || d->high > Q3) {
                if (d->high > Q2)
                    break;
            } else {
                d->low -= Q1;
                d->code -= Q1;
                d->high -= Q1;
            }
        } else {
            d->low -= Q2;
            d->code -= Q2;
            d->high -= Q2;
        }
        d->low *= 2;
        d->high *= 2;
        d->code = d->code * 2 + get_bit(d);
    }
}

int lzari_decode(const uint8_t *src, size_t srcn, uint8_t *dst, size_t dstn)
{
    Dec d;
    Model *m = malloc(sizeof *m);
    uint8_t *hist = malloc(HIST);
    size_t out = 0, hp = HIST - MAXM;
    int i;
    if (!m || !hist) {
        free(m);
        free(hist);
        return -1;
    }
    memset(&d, 0, sizeof d);
    d.p = src;
    d.n = srcn;
    d.high = Q4;
    model_init(m);
    memset(hist, ' ', HIST);
    for (i = 0; i < QB + 2; i++)
        d.code = d.code * 2 + get_bit(&d);
    while (out < dstn) {
        unsigned range = d.high - d.low, x;
        int ilo, ihi, sym, ch;
        if (d.at > srcn + 8)
            goto bad;                       /* ran far past the end: corrupt */
        x = (unsigned)(((uint64_t)(d.code - d.low + 1) * m->sym_cum[0] - 1) / range);
        ilo = 1;
        ihi = NCHAR;
        while (ilo < ihi) {
            int k = (ilo + ihi) / 2;
            if (m->sym_cum[k] > x)
                ilo = k + 1;
            else
                ihi = k;
        }
        sym = ilo;
        {
            unsigned h = d.low + (unsigned)((uint64_t)range * m->sym_cum[sym - 1] / m->sym_cum[0]);
            unsigned l = d.low + (unsigned)((uint64_t)range * m->sym_cum[sym] / m->sym_cum[0]);
            d.high = h;
            d.low = l;
        }
        dec_norm(&d);
        ch = m->sym_to_char[sym];
        model_update(m, sym);
        if (ch < 256) {
            dst[out++] = (uint8_t)ch;
            hist[hp] = (uint8_t)ch;
            hp = (hp + 1) % HIST;
        } else {
            int len = ch - 256 + MINM, pos, base, off;
            range = d.high - d.low;
            x = (unsigned)(((uint64_t)(d.code - d.low + 1) * m->pos_cum[0] - 1) / range);
            ilo = 1;
            ihi = HIST;
            while (ilo < ihi) {
                int k = (ilo + ihi) / 2;
                if (m->pos_cum[k] > x)
                    ilo = k + 1;
                else
                    ihi = k;
            }
            pos = ilo - 1;
            {
                unsigned h = d.low + (unsigned)((uint64_t)range * m->pos_cum[pos] / m->pos_cum[0]);
                unsigned l = d.low + (unsigned)((uint64_t)range * m->pos_cum[pos + 1] / m->pos_cum[0]);
                d.high = h;
                d.low = l;
            }
            dec_norm(&d);
            base = (int)((hp + HIST - (size_t)pos - 1) % HIST);
            for (off = 0; off < len && out < dstn; off++) {
                uint8_t c = hist[(base + off) % HIST];
                dst[out++] = c;
                hist[hp] = c;
                hp = (hp + 1) % HIST;
            }
        }
    }
    free(m);
    free(hist);
    return 0;
bad:
    free(m);
    free(hist);
    return -1;
}
