/*
 * ps2save.c - PS2 save containers (.psu .max .cbs .sps/.xps and raw memory card images) read
 * and written in memory. Layouts and sources: docs/formats/saves.md. The structure follows the
 * public domain notes in mymc (ps2save.py, ps2mc.py, ps2mc_ecc.py by Ross Ridge) and PCSX2's
 * memory card documentation; the code is written for this project.
 */
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ps2save.h"

#define DF_RWX 0x7
#define DF_FILE 0x10
#define DF_DIR 0x20
#define DF_0400 0x400
#define DF_HIDDEN 0x2000
#define DF_EXISTS 0x8000
#define MODE_DIR 0x8427
#define MODE_FILE 0x8497
#define IS_FILE(m) (((m) & (DF_FILE | DF_DIR | DF_EXISTS)) == (DF_FILE | DF_EXISTS))
#define IS_DIR(m) (((m) & (DF_FILE | DF_DIR | DF_EXISTS)) == (DF_DIR | DF_EXISTS))

static const char CARD_MAGIC[] = "Sony PS2 Memory Card Format ";
static const char MAX_MAGIC[] = "Ps2PowerSave";
static const char SPS_MAGIC[] = "\x0d\0\0\0SharkPortSave";   /* 17 bytes */
static const char CBS_MAGIC[] = "CFU\0";

static int fail(char *err, size_t n, const char *fmt, ...)
{
    if (err && n) {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(err, n, fmt, ap);
        va_end(ap);
    }
    return -1;
}

static void put_name(uint8_t *dst, size_t max, const char *name)
{
    size_t l = strlen(name);
    memcpy(dst, name, l < max ? l : max);
}

static uint32_t rd32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }
static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static void wr32(uint8_t *p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24); }
static void wr16(uint8_t *p, unsigned v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }

static void copy_name(char *dst, const uint8_t *src, size_t max)
{
    size_t i;
    for (i = 0; i < max && src[i]; i++)
        dst[i] = (char)src[i];
    dst[i < 32 ? i : 32] = 0;
}

uint32_t ps2s_crc32(uint32_t crc, const void *p, size_t n)
{
    static uint32_t tab[256];
    const uint8_t *b = p;
    if (!tab[1]) {
        uint32_t i, k, c;
        for (i = 0; i < 256; i++) {
            for (c = i, k = 0; k < 8; k++)
                c = c & 1 ? 0xEDB88320u ^ c >> 1 : c >> 1;
            tab[i] = c;
        }
    }
    crc = ~crc;
    while (n--)
        crc = tab[(crc ^ *b++) & 0xFF] ^ crc >> 8;
    return ~crc;
}

void ps2s_now(uint8_t *t)
{
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    memset(t, 0, 8);
    if (!tm)
        return;
    t[1] = (uint8_t)tm->tm_sec;
    t[2] = (uint8_t)tm->tm_min;
    t[3] = (uint8_t)tm->tm_hour;
    t[4] = (uint8_t)tm->tm_mday;
    t[5] = (uint8_t)(tm->tm_mon + 1);
    wr16(t + 6, (unsigned)(tm->tm_year + 1900));
}

void ps2s_free(Ps2sSave *s)
{
    int i;
    for (i = 0; i < s->nfiles; i++)
        free(s->files[i].data);
    free(s->files);
    memset(s, 0, sizeof *s);
}

Ps2sFile *ps2s_find(const Ps2sSave *s, const char *name)
{
    int i;
    for (i = 0; i < s->nfiles; i++)
        if (!strcmp(s->files[i].name, name))
            return &s->files[i];
    return NULL;
}

int ps2s_add(Ps2sSave *s, const char *name, const void *data, size_t n, const uint8_t *when)
{
    Ps2sFile *nf = realloc(s->files, (size_t)(s->nfiles + 1) * sizeof *nf), *f;
    if (!nf)
        return -1;
    s->files = nf;
    f = &nf[s->nfiles];
    memset(f, 0, sizeof *f);
    snprintf(f->name, sizeof f->name, "%s", name);
    f->mode = MODE_FILE;
    f->size = (uint32_t)n;
    f->data = malloc(n ? n : 1);
    if (!f->data)
        return -1;
    memcpy(f->data, data, n);
    if (when) {
        memcpy(f->ctime, when, 8);
        memcpy(f->mtime, when, 8);
    }
    s->nfiles++;
    return 0;
}

static int alloc_files(Ps2sSave *s, int n)
{
    s->nfiles = 0;
    s->files = n > 0 ? calloc((size_t)n, sizeof *s->files) : NULL;
    return n > 0 && !s->files ? -1 : 0;
}

/* ------------------------------------------------------------------ names */
int ps2s_format_from_name(const char *path)
{
    const char *e = strrchr(path, '.');
    char x[8];
    size_t i;
    if (!e || strlen(e) > 5)
        return PS2S_NONE;
    for (i = 0; e[i + 1] && i < 6; i++)
        x[i] = (char)tolower((unsigned char)e[i + 1]);
    x[i] = 0;
    if (!strcmp(x, "psu")) return PS2S_PSU;
    if (!strcmp(x, "max")) return PS2S_MAX;
    if (!strcmp(x, "cbs")) return PS2S_CBS;
    if (!strcmp(x, "sps") || !strcmp(x, "xps")) return PS2S_SPS;
    if (!strcmp(x, "ps2") || !strcmp(x, "mcd") || !strcmp(x, "mc2") || !strcmp(x, "bin") || !strcmp(x, "mc") || !strcmp(x, "vmc"))
        return PS2S_CARD;
    return PS2S_NONE;
}

const char *ps2s_format_name(int f)
{
    switch (f) {
    case PS2S_PSU: return "PSU (EMS / uLaunchELF)";
    case PS2S_MAX: return "MAX (Action Replay Max)";
    case PS2S_CBS: return "CBS (CodeBreaker)";
    case PS2S_SPS: return "SPS/XPS (SharkPort / X-Port)";
    case PS2S_CARD: return "memory card image";
    }
    return "unknown";
}

/* ------------------------------------------------------------------ detect */
static int looks_psu(const uint8_t *b, size_t n)
{
    if (n < 1536)
        return 0;
    return IS_DIR(rd16(b)) && IS_DIR(rd16(b + 512)) && IS_DIR(rd16(b + 1024)) && rd32(b + 4) >= 2
           && !strcmp((const char *)b + 512 + 0x40, ".") && !strcmp((const char *)b + 1024 + 0x40, "..");
}

int ps2s_detect(const uint8_t *b, size_t n)
{
    if (n >= 12 && !memcmp(b, MAX_MAGIC, 12)) return PS2S_MAX;
    if (n >= 17 && !memcmp(b, SPS_MAGIC, 17)) return PS2S_SPS;
    if (n >= 4 && !memcmp(b, CBS_MAGIC, 4)) return PS2S_CBS;
    if (n >= 28 && !memcmp(b, CARD_MAGIC, 28)) return PS2S_CARD;
    if (looks_psu(b, n)) return PS2S_PSU;
    return PS2S_NONE;
}

/* ------------------------------------------------------------------ PSU */
static void ent_unpack(const uint8_t *e, uint16_t *mode, uint32_t *size, uint8_t *ct, uint8_t *mt, char *name)
{
    *mode = rd16(e);
    *size = rd32(e + 4);
    memcpy(ct, e + 8, 8);
    memcpy(mt, e + 0x18, 8);
    copy_name(name, e + 0x40, 32);
}

static void ent_pack(uint8_t *e, uint16_t mode, uint32_t size, const uint8_t *ct, const uint8_t *mt, const char *name)
{
    memset(e, 0, 512);
    wr16(e, mode);
    wr32(e + 4, size);
    memcpy(e + 8, ct, 8);
    memcpy(e + 0x18, mt, 8);
    put_name(e + 0x40, 31, name);
}

static int read_psu(const uint8_t *b, size_t n, Ps2sSave *o, char *err, size_t en)
{
    uint32_t cnt, i, size;
    size_t pos = 1536;
    uint16_t mode;
    if (!looks_psu(b, n))
        return fail(err, en, "not a PSU save (no directory header)");
    ent_unpack(b, &o->mode, &cnt, o->ctime, o->mtime, o->dir);
    cnt -= 2;
    if (cnt > 256 || alloc_files(o, (int)cnt))
        return fail(err, en, "PSU: implausible file count %u", (unsigned)cnt);
    for (i = 0; i < cnt; i++) {
        Ps2sFile *f = &o->files[i];
        if (pos + 512 > n)
            return fail(err, en, "PSU: file ends inside the entry for file %u", (unsigned)i + 1);
        ent_unpack(b + pos, &mode, &size, f->ctime, f->mtime, f->name);
        if (!IS_FILE(mode))
            return fail(err, en, "PSU: entry '%s' is not a plain file", f->name);
        f->mode = mode;
        pos += 512;
        if (size > n - pos)
            return fail(err, en, "PSU: file '%s' is cut short (%u bytes stored)", f->name, (unsigned)(n - pos));
        f->size = size;
        f->data = malloc(size ? size : 1);
        if (!f->data)
            return fail(err, en, "out of memory");
        memcpy(f->data, b + pos, size);
        o->nfiles++;
        pos += (size + 1023) & ~1023u;
        if (pos > n)
            pos = n;
    }
    return 0;
}

static int write_psu(const Ps2sSave *s, uint8_t **out, size_t *outn)
{
    size_t total = 1536;
    uint8_t *o, *p;
    int i;
    for (i = 0; i < s->nfiles; i++)
        total += 512 + ((s->files[i].size + 1023) & ~1023u);
    o = calloc(total, 1);
    if (!o)
        return -1;
    p = o;
    ent_pack(p, s->mode ? s->mode : MODE_DIR, (uint32_t)s->nfiles + 2, s->ctime, s->mtime, s->dir);
    ent_pack(p + 512, MODE_DIR, 0, s->ctime, s->ctime, ".");
    ent_pack(p + 1024, MODE_DIR, 0, s->ctime, s->ctime, "..");
    p += 1536;
    for (i = 0; i < s->nfiles; i++) {
        const Ps2sFile *f = &s->files[i];
        ent_pack(p, f->mode ? f->mode : MODE_FILE, f->size, f->ctime, f->mtime, f->name);
        memcpy(p + 512, f->data, f->size);
        p += 512 + ((f->size + 1023) & ~1023u);
    }
    *out = o;
    *outn = total;
    return 0;
}

/* ------------------------------------------------------------------ inflate / stored zlib */
typedef struct {
    const uint8_t *in;
    size_t inlen, inpos;
    uint8_t *out;
    size_t outlen, outpos;
    unsigned bitbuf;
    int bitcnt;
    int err;
} Inf;

typedef struct { short count[16]; short symbol[288]; } Huff;

static int inf_bits(Inf *s, int need)
{
    long val = s->bitbuf;
    while (s->bitcnt < need) {
        if (s->inpos >= s->inlen) {
            s->err = 1;
            return 0;
        }
        val |= (long)s->in[s->inpos++] << s->bitcnt;
        s->bitcnt += 8;
    }
    s->bitbuf = (unsigned)(val >> need);
    s->bitcnt -= need;
    return (int)(val & ((1L << need) - 1));
}

static int huff_build(Huff *h, const short *len, int n)
{
    int sym, l, left;
    short offs[16];
    for (l = 0; l < 16; l++)
        h->count[l] = 0;
    for (sym = 0; sym < n; sym++)
        h->count[len[sym]]++;
    if (h->count[0] == n)
        return 0;
    left = 1;
    for (l = 1; l < 16; l++) {
        left <<= 1;
        left -= h->count[l];
        if (left < 0)
            return left;
    }
    offs[1] = 0;
    for (l = 1; l < 15; l++)
        offs[l + 1] = (short)(offs[l] + h->count[l]);
    for (sym = 0; sym < n; sym++)
        if (len[sym])
            h->symbol[offs[len[sym]]++] = (short)sym;
    return left;
}

static int huff_decode(Inf *s, const Huff *h)
{
    int code = 0, first = 0, index = 0, l;
    for (l = 1; l < 16; l++) {
        int count;
        code |= inf_bits(s, 1);
        if (s->err)
            return -1;
        count = h->count[l];
        if (code - count < first)
            return h->symbol[index + (code - first)];
        index += count;
        first += count;
        first <<= 1;
        code <<= 1;
    }
    return -1;
}

static int inf_codes(Inf *s, const Huff *lc, const Huff *dc)
{
    static const short lbase[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258 };
    static const short lext[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0 };
    static const short dbase[] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
    static const short dext[] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
    for (;;) {
        int sym = huff_decode(s, lc);
        if (sym < 0)
            return -1;
        if (sym < 256) {
            if (s->outpos >= s->outlen)
                return 1;                       /* output full: enough */
            s->out[s->outpos++] = (uint8_t)sym;
        } else if (sym == 256)
            return 0;
        else {
            int len, dist;
            sym -= 257;
            if (sym >= 29)
                return -1;
            len = lbase[sym] + inf_bits(s, lext[sym]);
            sym = huff_decode(s, dc);
            if (sym < 0 || sym >= 30)
                return -1;
            dist = dbase[sym] + inf_bits(s, dext[sym]);
            if (s->err || (size_t)dist > s->outpos)
                return -1;
            while (len--) {
                if (s->outpos >= s->outlen)
                    return 1;
                s->out[s->outpos] = s->out[s->outpos - (size_t)dist];
                s->outpos++;
            }
        }
    }
}

/* raw deflate stream -> out (outlen bytes wanted); returns bytes produced or -1 */
static long inflate_raw(const uint8_t *in, size_t inlen, uint8_t *out, size_t outlen)
{
    Inf s;
    int last;
    memset(&s, 0, sizeof s);
    s.in = in; s.inlen = inlen; s.out = out; s.outlen = outlen;
    do {
        int type, r = 0;
        last = inf_bits(&s, 1);
        type = inf_bits(&s, 2);
        if (s.err)
            return -1;
        if (type == 0) {
            unsigned len, nlen;
            s.bitbuf = 0;
            s.bitcnt = 0;
            if (s.inpos + 4 > s.inlen)
                return -1;
            len = rd16(s.in + s.inpos);
            nlen = rd16(s.in + s.inpos + 2);
            s.inpos += 4;
            if (len != (~nlen & 0xFFFF) || s.inpos + len > s.inlen)
                return -1;
            if (len > s.outlen - s.outpos) {
                len = (unsigned)(s.outlen - s.outpos);
                r = 1;
            }
            memcpy(s.out + s.outpos, s.in + s.inpos, len);
            s.outpos += len;
            s.inpos += len;
        } else if (type == 1 || type == 2) {
            Huff lc, dc;
            short lengths[320];
            int i;
            if (type == 1) {
                for (i = 0; i < 144; i++) lengths[i] = 8;
                for (; i < 256; i++) lengths[i] = 9;
                for (; i < 280; i++) lengths[i] = 7;
                for (; i < 288; i++) lengths[i] = 8;
                huff_build(&lc, lengths, 288);
                for (i = 0; i < 30; i++) lengths[i] = 5;
                huff_build(&dc, lengths, 30);
            } else {
                static const short order[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
                int nlen = inf_bits(&s, 5) + 257, ndist = inf_bits(&s, 5) + 1, ncode = inf_bits(&s, 4) + 4, idx;
                if (s.err || nlen > 286 || ndist > 30)
                    return -1;
                for (idx = 0; idx < 19; idx++)
                    lengths[idx] = 0;
                for (idx = 0; idx < ncode; idx++)
                    lengths[order[idx]] = (short)inf_bits(&s, 3);
                if (huff_build(&lc, lengths, 19) != 0)
                    return -1;
                idx = 0;
                while (idx < nlen + ndist) {
                    int sym = huff_decode(&s, &lc), len = 0, rep;
                    if (sym < 0)
                        return -1;
                    if (sym < 16)
                        lengths[idx++] = (short)sym;
                    else {
                        if (sym == 16) {
                            if (idx == 0)
                                return -1;
                            len = lengths[idx - 1];
                            rep = 3 + inf_bits(&s, 2);
                        } else if (sym == 17)
                            rep = 3 + inf_bits(&s, 3);
                        else
                            rep = 11 + inf_bits(&s, 7);
                        if (idx + rep > nlen + ndist)
                            return -1;
                        while (rep--)
                            lengths[idx++] = (short)len;
                    }
                }
                if (s.err)
                    return -1;
                huff_build(&lc, lengths, nlen);
                huff_build(&dc, lengths + nlen, ndist);
            }
            r = inf_codes(&s, &lc, &dc);
            if (r < 0)
                return -1;
        } else
            return -1;
        if (r == 1)
            break;                              /* output full */
    } while (!last);
    return (long)s.outpos;
}

static uint32_t adler32(const uint8_t *p, size_t n)
{
    uint32_t a = 1, b = 0;
    while (n--) {
        a = (a + *p++) % 65521;
        b = (b + a) % 65521;
    }
    return b << 16 | a;
}

/* zlib stream of stored (uncompressed) blocks: valid for every inflater */
static uint8_t *zlib_store(const uint8_t *src, size_t n, size_t *outn)
{
    size_t blocks = n ? (n + 65534) / 65535 : 1, i, at = 2;
    uint8_t *o = malloc(2 + blocks * 5 + n + 4);
    if (!o)
        return NULL;
    o[0] = 0x78;
    o[1] = 0x01;
    for (i = 0; i < blocks; i++) {
        size_t len = n > 65535 ? 65535 : n;
        o[at++] = i == blocks - 1;
        wr16(o + at, (unsigned)len);
        wr16(o + at + 2, ~len & 0xFFFF);
        at += 4;
        memcpy(o + at, src, len);
        at += len;
        src += len;
        n -= len;
    }
    *outn = at + 4;
    return o;
}

/* ------------------------------------------------------------------ MAX */
static int read_max(const uint8_t *b, size_t n, Ps2sSave *o, char *err, size_t en)
{
    uint32_t crc, clen, dirlen, length, off = 0, i;
    size_t cbytes;
    uint8_t *raw;
    uint8_t hdr[0x5C];
    uint32_t calc;
    if (n < 0x5C || memcmp(b, MAX_MAGIC, 12))
        return fail(err, en, "not a MAX Drive save");
    crc = rd32(b + 12);
    clen = rd32(b + 0x50);
    dirlen = rd32(b + 0x54);
    length = rd32(b + 0x58);
    if (dirlen > 256 || length > (64u << 20))
        return fail(err, en, "MAX: implausible sizes (files %u, data %u)", (unsigned)dirlen, (unsigned)length);
    cbytes = clen == length ? n - 0x5C : (clen >= 4 ? clen - 4 : 0);
    if (cbytes > n - 0x5C)
        return fail(err, en, "MAX: file is cut short (%u of %u compressed bytes)", (unsigned)(n - 0x5C), (unsigned)cbytes);
    memcpy(hdr, b, 0x5C);
    wr32(hdr + 12, 0);
    calc = ps2s_crc32(ps2s_crc32(0, hdr, 0x5C), b + 0x5C, cbytes);
    raw = malloc(length ? length : 1);
    if (!raw)
        return fail(err, en, "out of memory");
    if (lzari_decode(b + 0x5C, cbytes, raw, length) != 0) {
        free(raw);
        return fail(err, en, "MAX: compressed data is corrupt");
    }
    copy_name(o->dir, b + 0x10, 32);
    o->mode = MODE_DIR;
    ps2s_now(o->ctime);
    memcpy(o->mtime, o->ctime, 8);
    if (alloc_files(o, (int)dirlen)) {
        free(raw);
        return fail(err, en, "out of memory");
    }
    for (i = 0; i < dirlen; i++) {
        uint32_t l;
        Ps2sFile *f = &o->files[i];
        if (length - off < 36 || (l = rd32(raw + off), l > length - off - 36)) {
            free(raw);
            return fail(err, en, "MAX: file table is cut short");
        }
        copy_name(f->name, raw + off + 4, 32);
        off += 36;
        f->mode = MODE_FILE;
        f->size = l;
        f->data = malloc(l ? l : 1);
        if (!f->data) {
            free(raw);
            return fail(err, en, "out of memory");
        }
        memcpy(f->data, raw + off, l);
        memcpy(f->ctime, o->ctime, 8);
        memcpy(f->mtime, o->ctime, 8);
        o->nfiles++;
        off += l;
        off = ((off + 8 + 15) & ~15u) - 8;
    }
    free(raw);
    if (calc != crc && err && en)
        snprintf(err, en, "note: the MAX header CRC does not match (%08X, file says %08X); contents read anyway", (unsigned)calc, (unsigned)crc);
    return 0;
}

static int write_max(const Ps2sSave *s, uint8_t **out, size_t *outn, char *err, size_t en)
{
    size_t total = 0, off = 0, clen;
    uint8_t *raw, *comp, *o;
    int i;
    for (i = 0; i < s->nfiles; i++)
        total += 36 + s->files[i].size + 16;
    raw = calloc(total, 1);
    if (!raw)
        return fail(err, en, "out of memory");
    for (i = 0; i < s->nfiles; i++) {
        const Ps2sFile *f = &s->files[i];
        wr32(raw + off, f->size);
        put_name(raw + off + 4, 31, f->name);
        memcpy(raw + off + 36, f->data, f->size);
        off += 36 + f->size;
        off = ((off + 8 + 15) & ~15u) - 8;
    }
    comp = lzari_encode(raw, off, &clen);
    if (!comp) {
        free(raw);
        return fail(err, en, "MAX: compression failed");
    }
    o = calloc(0x5C + clen, 1);
    if (!o) {
        free(raw); free(comp);
        return fail(err, en, "out of memory");
    }
    memcpy(o, MAX_MAGIC, 12);
    put_name(o + 0x10, 31, s->dir);
    put_name(o + 0x30, 31, !strcmp(s->dir, "BISLPM-65495MH") ? "Monster Hunter" : s->dir);
    wr32(o + 0x50, (uint32_t)clen + 4);
    wr32(o + 0x54, (uint32_t)s->nfiles);
    wr32(o + 0x58, (uint32_t)off);
    memcpy(o + 0x5C, comp, clen);
    wr32(o + 12, ps2s_crc32(0, o, 0x5C + clen));    /* the CRC field is 0 while it is computed */
    free(raw);
    free(comp);
    *out = o;
    *outn = 0x5C + clen;
    return 0;
}

/* ------------------------------------------------------------------ CBS */
static const uint8_t CBS_RC4S[256] = {
    0x5f, 0x1f, 0x85, 0x6f, 0x31, 0xaa, 0x3b, 0x18, 0x21, 0xb9, 0xce, 0x1c, 0x07, 0x4c, 0x9c, 0xb4,
    0x81, 0xb8, 0xef, 0x98, 0x59, 0xae, 0xf9, 0x26, 0xe3, 0x80, 0xa3, 0x29, 0x2d, 0x73, 0x51, 0x62,
    0x7c, 0x64, 0x46, 0xf4, 0x34, 0x1a, 0xf6, 0xe1, 0xba, 0x3a, 0x0d, 0x82, 0x79, 0x0a, 0x5c, 0x16,
    0x71, 0x49, 0x8e, 0xac, 0x8c, 0x9f, 0x35, 0x19, 0x45, 0x94, 0x3f, 0x56, 0x0c, 0x91, 0x00, 0x0b,
    0xd7, 0xb0, 0xdd, 0x39, 0x66, 0xa1, 0x76, 0x52, 0x13, 0x57, 0xf3, 0xbb, 0x4e, 0xe5, 0xdc, 0xf0,
    0x65, 0x84, 0xb2, 0xd6, 0xdf, 0x15, 0x3c, 0x63, 0x1d, 0x89, 0x14, 0xbd, 0xd2, 0x36, 0xfe, 0xb1,
    0xca, 0x8b, 0xa4, 0xc6, 0x9e, 0x67, 0x47, 0x37, 0x42, 0x6d, 0x6a, 0x03, 0x92, 0x70, 0x05, 0x7d,
    0x96, 0x2f, 0x40, 0x90, 0xc4, 0xf1, 0x3e, 0x3d, 0x01, 0xf7, 0x68, 0x1e, 0xc3, 0xfc, 0x72, 0xb5,
    0x54, 0xcf, 0xe7, 0x41, 0xe4, 0x4d, 0x83, 0x55, 0x12, 0x22, 0x09, 0x78, 0xfa, 0xde, 0xa7, 0x06,
    0x08, 0x23, 0xbf, 0x0f, 0xcc, 0xc1, 0x97, 0x61, 0xc5, 0x4a, 0xe6, 0xa0, 0x11, 0xc2, 0xea, 0x74,
    0x02, 0x87, 0xd5, 0xd1, 0x9d, 0xb7, 0x7e, 0x38, 0x60, 0x53, 0x95, 0x8d, 0x25, 0x77, 0x10, 0x5e,
    0x9b, 0x7f, 0xd8, 0x6e, 0xda, 0xa2, 0x2e, 0x20, 0x4f, 0xcd, 0x8f, 0xcb, 0xbe, 0x5a, 0xe0, 0xed,
    0x2c, 0x9a, 0xd4, 0xe2, 0xaf, 0xd0, 0xa9, 0xe8, 0xad, 0x7a, 0xbc, 0xa8, 0xf2, 0xee, 0xeb, 0xf5,
    0xa6, 0x99, 0x28, 0x24, 0x6c, 0x2b, 0x75, 0x5d, 0xf8, 0xd3, 0x86, 0x17, 0xfb, 0xc0, 0x7b, 0xb3,
    0x58, 0xdb, 0xc7, 0x4b, 0xff, 0x04, 0x50, 0xe9, 0x88, 0x69, 0xc9, 0x2a, 0xab, 0xfd, 0x5b, 0x1b,
    0x8a, 0xd9, 0xec, 0x27, 0x44, 0x0e, 0x33, 0xc8, 0x6b, 0x93, 0x32, 0x48, 0xb6, 0x30, 0x43, 0xa5
};

/* CodeBreaker's RC4 variant: the key schedule is skipped, the fixed table above is the state */
static void cbs_crypt(uint8_t *t, size_t n)
{
    uint8_t s[256];
    unsigned i, j = 0;
    size_t k;
    memcpy(s, CBS_RC4S, 256);
    for (k = 0; k < n; k++) {
        uint8_t x;
        i = (unsigned)((k + 1) % 256);
        j = (j + s[i]) & 255;
        x = s[i]; s[i] = s[j]; s[j] = x;
        t[k] ^= s[(s[i] + s[j]) & 255];
    }
}

static int read_cbs(const uint8_t *b, size_t n, Ps2sSave *o, char *err, size_t en)
{
    uint32_t hlen, dlen, flen;
    size_t clen, off = 0;
    uint8_t *body, *dec;
    long got;
    int cnt = 0, i;
    if (n < 12 || memcmp(b, CBS_MAGIC, 4))
        return fail(err, en, "not a CodeBreaker save");
    hlen = rd32(b + 8);
    if (hlen < 92 + 32 || hlen > n)
        return fail(err, en, "CBS: bad header length %u", (unsigned)hlen);
    dlen = rd32(b + 12);
    flen = rd32(b + 16);
    if (dlen > (64u << 20))
        return fail(err, en, "CBS: implausible size");
    copy_name(o->dir, b + 20, 32);
    memcpy(o->ctime, b + 52, 8);
    memcpy(o->mtime, b + 60, 8);
    o->mode = IS_DIR(rd32(b + 76)) ? (uint16_t)rd32(b + 76) : MODE_DIR;
    /* flen is the whole file's length or just the body's */
    clen = flen >= n ? n - hlen : (flen <= n - hlen ? flen : n - hlen);
    if (flen == n - hlen || flen == n)
        clen = n - hlen;
    body = malloc(clen ? clen : 1);
    dec = malloc(dlen ? dlen : 1);
    if (!body || !dec) {
        free(body); free(dec);
        return fail(err, en, "out of memory");
    }
    memcpy(body, b + hlen, clen);
    cbs_crypt(body, clen);
    if (clen < 6 || (body[0] & 0x0F) != 8) {
        free(body); free(dec);
        return fail(err, en, "CBS: decrypted data is not zlib (damaged file?)");
    }
    got = inflate_raw(body + 2, clen - 2, dec, dlen);
    free(body);
    if (got < 0 || (uint32_t)got != dlen) {
        free(dec);
        return fail(err, en, "CBS: decompression failed (%ld of %u bytes)", got, (unsigned)dlen);
    }
    for (off = 0; off + 64 <= dlen; ) {
        uint32_t sz = rd32(dec + off + 16);
        if (sz > dlen - off - 64)
            break;
        off += 64 + sz;
        cnt++;
    }
    if (off != dlen || cnt > 256 || alloc_files(o, cnt)) {
        free(dec);
        return fail(err, en, "CBS: file table is damaged");
    }
    for (off = 0, i = 0; i < cnt; i++) {
        Ps2sFile *f = &o->files[i];
        uint32_t sz = rd32(dec + off + 16);
        memcpy(f->ctime, dec + off, 8);
        memcpy(f->mtime, dec + off + 8, 8);
        f->size = sz;
        f->mode = rd16(dec + off + 20);
        if (!IS_FILE(f->mode))
            f->mode = MODE_FILE;
        copy_name(f->name, dec + off + 32, 32);
        f->data = malloc(sz ? sz : 1);
        if (!f->data) {
            free(dec);
            return fail(err, en, "out of memory");
        }
        memcpy(f->data, dec + off + 64, sz);
        off += 64 + sz;
        o->nfiles++;
    }
    free(dec);
    return 0;
}

static int write_cbs(const Ps2sSave *s, uint8_t **out, size_t *outn, char *err, size_t en)
{
    size_t dlen = 0, off = 0, zl;
    uint8_t *raw, *z, *o;
    uint32_t ad;
    int i;
    const uint32_t hlen = 0x128;
    for (i = 0; i < s->nfiles; i++)
        dlen += 64 + s->files[i].size;
    raw = calloc(dlen ? dlen : 1, 1);
    if (!raw)
        return fail(err, en, "out of memory");
    for (i = 0; i < s->nfiles; i++) {
        const Ps2sFile *f = &s->files[i];
        memcpy(raw + off, f->ctime, 8);
        memcpy(raw + off + 8, f->mtime, 8);
        wr32(raw + off + 16, f->size);
        wr16(raw + off + 20, f->mode ? f->mode : MODE_FILE);
        put_name(raw + off + 32, 31, f->name);
        memcpy(raw + off + 64, f->data, f->size);
        off += 64 + f->size;
    }
    z = zlib_store(raw, dlen, &zl);
    if (!z) {
        free(raw);
        return fail(err, en, "out of memory");
    }
    ad = adler32(raw, dlen);
    z[zl - 4] = (uint8_t)(ad >> 24); z[zl - 3] = (uint8_t)(ad >> 16); z[zl - 2] = (uint8_t)(ad >> 8); z[zl - 1] = (uint8_t)ad;
    free(raw);
    cbs_crypt(z, zl);
    o = calloc(hlen + zl, 1);
    if (!o) {
        free(z);
        return fail(err, en, "out of memory");
    }
    memcpy(o, CBS_MAGIC, 4);
    wr32(o + 4, 0x10);                  /* unknown, not read by mymc */
    wr32(o + 8, hlen);
    wr32(o + 12, (uint32_t)dlen);
    wr32(o + 16, (uint32_t)(hlen + zl)); /* the whole file's length */
    put_name(o + 20, 31, s->dir);
    memcpy(o + 52, s->ctime, 8);
    memcpy(o + 60, s->mtime, 8);
    wr32(o + 76, s->mode ? s->mode : MODE_DIR);
    put_name(o + 92, 31, !strcmp(s->dir, "BISLPM-65495MH") ? "Monster Hunter" : s->dir);
    memcpy(o + hlen, z, zl);
    free(z);
    *out = o;
    *outn = hlen + zl;
    return 0;
}

/* ------------------------------------------------------------------ SPS / XPS */
static int read_sps(const uint8_t *b, size_t n, Ps2sSave *o, char *err, size_t en)
{
    size_t pos = 17;
    uint32_t len, cnt, i, size;
    int k;
    if (n < 17 + 4 || memcmp(b, SPS_MAGIC, 17))
        return fail(err, en, "not a SharkPort / X-Port save");
    pos += 4;                               /* save type */
    for (k = 0; k < 3; k++) {               /* directory name, date, comment: length-prefixed */
        if (pos + 4 > n || (len = rd32(b + pos), len > n - pos - 4))
            return fail(err, en, "SPS: header is cut short");
        pos += 4 + len;
    }
    pos += 4;                               /* length of the rest */
#define SPSHDR(f, nm, sz, mode, ct, mt)                                                     \
    do {                                                                                    \
        uint16_t hl;                                                                        \
        if (pos + 98 > n)                                                                   \
            return fail(err, en, "SPS: file ends inside a header");                         \
        hl = rd16(b + pos);                                                                 \
        copy_name(nm, b + pos + 2, 64);                                                     \
        sz = rd32(b + pos + 66);                                                            \
        mode = (uint16_t)(b[pos + 78] << 8 | b[pos + 79]);                                  \
        memcpy(ct, b + pos + 82, 8);                                                        \
        memcpy(mt, b + pos + 90, 8);                                                        \
        if (hl < 98 || pos + hl > n)                                                        \
            return fail(err, en, "SPS: bad header length");                                 \
        pos += hl;                                                                          \
    } while (0)
    {
        char nm[65];
        uint16_t mode;
        SPSHDR(0, nm, cnt, mode, o->ctime, o->mtime);
        if (!IS_DIR(mode) || cnt < 2 || cnt - 2 > 256)
            return fail(err, en, "SPS: bad directory entry");
        snprintf(o->dir, sizeof o->dir, "%.32s", nm);
        o->mode = mode;
        cnt -= 2;
    }
    if (alloc_files(o, (int)cnt))
        return fail(err, en, "out of memory");
    for (i = 0; i < cnt; i++) {
        Ps2sFile *f = &o->files[i];
        char nm[65];
        uint16_t mode;
        SPSHDR(f, nm, size, mode, f->ctime, f->mtime);
        if (!IS_FILE(mode))
            return fail(err, en, "SPS: '%s' is not a plain file", nm);
        if (size > n - pos)
            return fail(err, en, "SPS: file '%s' is cut short", nm);
        snprintf(f->name, sizeof f->name, "%.32s", nm);
        f->mode = mode;
        f->size = size;
        f->data = malloc(size ? size : 1);
        if (!f->data)
            return fail(err, en, "out of memory");
        memcpy(f->data, b + pos, size);
        pos += size;
        o->nfiles++;
    }
#undef SPSHDR
    return 0;                               /* the 4 byte checksum after the last file is not checked */
}

static void sps_hdr(uint8_t *p, const char *name, uint32_t len, uint16_t mode, const uint8_t *ct, const uint8_t *mt)
{
    memset(p, 0, 98);
    wr16(p, 98);
    put_name(p + 2, 63, name);
    wr32(p + 66, len);
    p[78] = (uint8_t)(mode >> 8);            /* the mode is stored byte swapped */
    p[79] = (uint8_t)mode;
    memcpy(p + 82, ct, 8);
    memcpy(p + 90, mt, 8);
}

static int write_sps(const Ps2sSave *s, uint8_t **out, size_t *outn, char *err, size_t en)
{
    size_t body = 98, total, at = 0;
    uint8_t *o;
    int i;
    char stamp[32];
    const char *comment = "Monster Hunter";
    uint32_t h = 0;
    size_t k;
    for (i = 0; i < s->nfiles; i++)
        body += 98 + s->files[i].size;
    snprintf(stamp, sizeof stamp, "%04u%02u%02u%02u%02u%02u", rd16(s->mtime + 6), s->mtime[5], s->mtime[4], s->mtime[3], s->mtime[2], s->mtime[1]);
    total = 17 + 4 + 4 + strlen(s->dir) + 4 + strlen(stamp) + 4 + strlen(comment) + 4 + body + 4;
    o = calloc(total, 1);
    if (!o)
        return fail(err, en, "out of memory");
    memcpy(o, SPS_MAGIC, 17);
    at = 17;
    wr32(o + at, 2);                        /* save type: PS2 */
    at += 4;
    wr32(o + at, (uint32_t)strlen(s->dir)); memcpy(o + at + 4, s->dir, strlen(s->dir)); at += 4 + strlen(s->dir);
    wr32(o + at, (uint32_t)strlen(stamp)); memcpy(o + at + 4, stamp, strlen(stamp)); at += 4 + strlen(stamp);
    wr32(o + at, (uint32_t)strlen(comment)); memcpy(o + at + 4, comment, strlen(comment)); at += 4 + strlen(comment);
    wr32(o + at, (uint32_t)body);
    at += 4;
    k = at;
    sps_hdr(o + at, s->dir, (uint32_t)s->nfiles + 2, s->mode ? s->mode : MODE_DIR, s->ctime, s->mtime);
    at += 98;
    for (i = 0; i < s->nfiles; i++) {
        const Ps2sFile *f = &s->files[i];
        sps_hdr(o + at, f->name, f->size, f->mode ? f->mode : MODE_FILE, f->ctime, f->mtime);
        at += 98;
        memcpy(o + at, f->data, f->size);
        at += f->size;
    }
    for (; k < at; k++)                     /* checksum as in mymc's commented sps_check; not verified by readers */
        h = (h + ((uint32_t)o[k] << (h % 24))) & 0xFFFFFFFFu;
    wr32(o + at, h);
    *out = o;
    *outn = total;
    return 0;
}

/* ------------------------------------------------------------------ memory card images */
typedef struct {
    const uint8_t *b;
    size_t n;
    unsigned page, spare, ppc, cs, epc;
    uint32_t clusters, alloc_off, alloc_end, root, ifc[32];
} Card;

static int card_open(Card *c, const uint8_t *b, size_t n, char *err, size_t en)
{
    unsigned raw;
    int i;
    if (n < 0x154 || memcmp(b, CARD_MAGIC, 28))
        return fail(err, en, "not a PS2 memory card image");
    memset(c, 0, sizeof *c);
    c->b = b;
    c->n = n;
    c->page = rd16(b + 0x28);
    c->ppc = rd16(b + 0x2A);
    c->clusters = rd32(b + 0x30);
    c->alloc_off = rd32(b + 0x34);
    c->alloc_end = rd32(b + 0x38);
    c->root = rd32(b + 0x3C);
    for (i = 0; i < 32; i++)
        c->ifc[i] = rd32(b + 0x50 + 4 * i);
    if (c->page < 512 || c->page > 4096 || c->ppc < 1 || c->ppc > 8 || c->page * c->ppc != 1024 || c->clusters < 64)
        return fail(err, en, "memory card: unsupported geometry (page %u x %u)", c->page, c->ppc);
    c->cs = c->page * c->ppc;
    c->epc = c->cs / 4;
    c->spare = (c->page + 127) / 128 * 4;
    raw = c->page + c->spare;
    if (n == (size_t)c->clusters * c->cs)
        c->spare = 0;                       /* no ECC bytes in the image */
    else if (n != (size_t)c->clusters * c->ppc * raw)
        return fail(err, en, "memory card: image size %u does not match its header (%u clusters)", (unsigned)n, (unsigned)c->clusters);
    return 0;
}

static int card_read_cluster(const Card *c, uint32_t n, uint8_t *dst)
{
    unsigned p;
    if (n >= c->clusters)
        return -1;
    if (!c->spare) {
        memcpy(dst, c->b + (size_t)n * c->cs, c->cs);
        return 0;
    }
    for (p = 0; p < c->ppc; p++)
        memcpy(dst + p * c->page, c->b + ((size_t)n * c->ppc + p) * (c->page + c->spare), c->page);
    return 0;
}

static int card_fat(const Card *c, uint32_t n, uint32_t *val)
{
    uint8_t buf[4096];
    uint32_t f = n / c->epc, ic = f / c->epc, fc;
    if (ic >= 32 || !c->ifc[ic] || card_read_cluster(c, c->ifc[ic], buf))
        return -1;
    fc = rd32(buf + 4 * (f % c->epc));
    if (card_read_cluster(c, fc, buf))
        return -1;
    *val = rd32(buf + 4 * (n % c->epc));
    return 0;
}

/* len bytes of the cluster chain starting at `first`, into dst */
static int card_read_chain(const Card *c, uint32_t first, size_t len, uint8_t *dst)
{
    uint8_t buf[4096];
    uint32_t cl = first;
    size_t done = 0, steps = 0;
    while (done < len) {
        size_t take = len - done < c->cs ? len - done : c->cs;
        uint32_t next;
        if (cl >= c->alloc_end || steps++ > c->alloc_end)
            return -1;
        if (card_read_cluster(c, cl + c->alloc_off, buf))
            return -1;
        memcpy(dst + done, buf, take);
        done += take;
        if (done >= len)
            break;
        if (card_fat(c, cl, &next) || next == 0xFFFFFFFFu || !(next & 0x80000000u))
            return -1;
        cl = next & 0x7FFFFFFFu;
    }
    return 0;
}

static uint8_t *card_dir(const Card *c, uint32_t first, uint32_t count, char *err, size_t en)
{
    uint8_t *d;
    if (count == 0 || count > 8192)
        return (void)fail(err, en, "memory card: bad directory size %u", (unsigned)count), NULL;
    d = malloc((size_t)count * 512 + 4096);
    if (!d)
        return (void)fail(err, en, "out of memory"), NULL;
    if (card_read_chain(c, first, (size_t)count * 512, d)) {
        free(d);
        return (void)fail(err, en, "memory card: directory chain is damaged"), NULL;
    }
    return d;
}

int ps2s_card_dirs(const uint8_t *b, size_t n, char *list, size_t listn)
{
    Card c;
    uint8_t *root;
    uint32_t cnt, i;
    size_t at = 0;
    char err[128];
    if (list && listn)
        list[0] = 0;
    if (card_open(&c, b, n, err, sizeof err))
        return -1;
    {
        uint8_t first[4096];
        if (card_read_cluster(&c, c.root + c.alloc_off, first))
            return -1;
        cnt = rd32(first + 4);
    }
    root = card_dir(&c, c.root, cnt, err, sizeof err);
    if (!root)
        return -1;
    for (i = 2; i < cnt; i++) {
        const uint8_t *e = root + 512 * i;
        if (IS_DIR(rd16(e)) && list && at + 40 < listn)
            at += (size_t)snprintf(list + at, listn - at, "%s%s", at ? ", " : "", (const char *)e + 0x40);
    }
    free(root);
    return 0;
}

static int read_card(const uint8_t *b, size_t n, const char *want, Ps2sSave *o, char *err, size_t en)
{
    Card c;
    uint8_t *root, *dir = NULL, first[4096];
    uint32_t cnt, i, dcnt, dfirst = 0;
    int found = -1;
    if (card_open(&c, b, n, err, en))
        return -1;
    if (card_read_cluster(&c, c.root + c.alloc_off, first))
        return fail(err, en, "memory card: root directory is outside the image");
    if (!IS_DIR(rd16(first)) || strcmp((const char *)first + 0x40, "."))
        return fail(err, en, "memory card: root directory is damaged");
    cnt = rd32(first + 4);
    if (!(root = card_dir(&c, c.root, cnt, err, en)))
        return -1;
    for (i = 2; i < cnt; i++) {
        const uint8_t *e = root + 512 * i;
        if (!IS_DIR(rd16(e)))
            continue;
        if (!want || !strncmp((const char *)e + 0x40, want, 32)) {
            found = (int)i;
            break;
        }
    }
    if (found < 0) {
        char names[400];
        ps2s_card_dirs(b, n, names, sizeof names);
        free(root);
        return fail(err, en, "the memory card has no save '%s' (found: %s)", want ? want : "?", names[0] ? names : "nothing");
    }
    {
        const uint8_t *e = root + 512 * found;
        ent_unpack(e, &o->mode, &dcnt, o->ctime, o->mtime, o->dir);
        dfirst = rd32(e + 0x10);
    }
    free(root);
    if (dcnt < 2 || dcnt > 258)
        return fail(err, en, "memory card: bad save directory size %u", (unsigned)dcnt);
    if (!(dir = card_dir(&c, dfirst, dcnt, err, en)))
        return -1;
    if (alloc_files(o, (int)dcnt - 2)) {
        free(dir);
        return fail(err, en, "out of memory");
    }
    for (i = 2; i < dcnt; i++) {
        const uint8_t *e = dir + 512 * i;
        Ps2sFile *f = &o->files[o->nfiles];
        uint16_t mode;
        uint32_t size;
        if (!(rd16(e) & DF_EXISTS))
            continue;
        ent_unpack(e, &mode, &size, f->ctime, f->mtime, f->name);
        if (!IS_FILE(mode)) {
            free(dir);
            return fail(err, en, "memory card: '%s' inside the save is not a plain file", f->name);
        }
        f->mode = mode;
        f->size = size;
        f->data = malloc(size ? size : 1);
        if (!f->data || (size && card_read_chain(&c, rd32(e + 0x10), size, f->data))) {
            free(dir);
            return fail(err, en, "memory card: the chain of '%s' is damaged", f->name);
        }
        o->nfiles++;
    }
    free(dir);
    return 0;
}

/* ---- ECC (Hamming code per 128 bytes, three bytes) and a fresh card */
static void ecc128(const uint8_t *s, uint8_t *out)
{
    static uint8_t par[256], cpm[256];
    static const uint8_t masks[7] = { 0x55, 0x33, 0x0F, 0x00, 0xAA, 0xCC, 0xF0 };
    unsigned cp = 0x77, lp0 = 0x7F, lp1 = 0x7F, i;
    if (!par[1]) {
        unsigned b, k;
        for (b = 0; b < 256; b++) {
            unsigned a = b ^ b >> 1;
            a ^= a >> 2;
            a ^= a >> 4;
            par[b] = (uint8_t)(a & 1);
        }
        for (b = 0; b < 256; b++) {
            unsigned m = 0;
            for (k = 0; k < 7; k++)
                m |= (unsigned)par[b & masks[k]] << k;
            cpm[b] = (uint8_t)m;
        }
    }
    for (i = 0; i < 128; i++) {
        cp ^= cpm[s[i]];
        if (par[s[i]]) {
            lp0 ^= ~i;
            lp1 ^= i;
        }
    }
    out[0] = (uint8_t)cp;
    out[1] = (uint8_t)(lp0 & 0x7F);
    out[2] = (uint8_t)lp1;
}

#define CARD_CLUSTERS 8192
#define CARD_FIRST_IFC 8
#define CARD_FAT_CLUSTERS 32
#define CARD_ALLOC_OFF 41               /* 8 + 1 indirect + 32 FAT */
#define CARD_ALLOC_END 8135             /* clusters_per_erase_block 8 * (1024 - 2) - 41 */

static void put_ent(uint8_t *e, uint16_t mode, uint32_t len, const uint8_t *ct, const uint8_t *mt, uint32_t cluster, uint32_t dirent, const char *name)
{
    ent_pack(e, mode, len, ct, mt, name);
    wr32(e + 0x10, cluster);
    wr32(e + 0x14, dirent);
}

static int write_card(const Ps2sSave *s, uint8_t **out, size_t *outn, char *err, size_t en)
{
    uint8_t *cook, *img;
    uint32_t *fat;
    uint32_t next = 0, root_cl, dir_cl, i, c;
    unsigned dir_ents = (unsigned)s->nfiles + 2, dir_cls = (dir_ents + 1) / 2, root_cls = 2;
    size_t p, total_clusters = 0;
    uint8_t zero[8] = { 0 }, now[8];
    uint8_t *ifc, *root, *dir;
    size_t cook_sz = (size_t)CARD_CLUSTERS * 1024;
    int k;

    for (k = 0; k < s->nfiles; k++)
        total_clusters += (s->files[k].size + 1023) / 1024;
    if (root_cls + dir_cls + total_clusters > CARD_ALLOC_END)
        return fail(err, en, "the save does not fit on a memory card");
    cook = calloc(cook_sz, 1);
    fat = malloc(CARD_CLUSTERS * sizeof *fat);
    if (!cook || !fat) {
        free(cook); free(fat);
        return fail(err, en, "out of memory");
    }
    ps2s_now(now);
    for (i = 0; i < CARD_CLUSTERS; i++)
        fat[i] = i < CARD_ALLOC_END ? 0x7FFFFFFFu : 0xFFFFFFFFu;
    /* superblock */
    memcpy(cook, CARD_MAGIC, 28);
    memcpy(cook + 28, "1.2.0.0", 7);
    wr16(cook + 0x28, 512);
    wr16(cook + 0x2A, 2);
    wr16(cook + 0x2C, 16);
    wr16(cook + 0x2E, 0xFF00);
    wr32(cook + 0x30, CARD_CLUSTERS);
    wr32(cook + 0x34, CARD_ALLOC_OFF);
    wr32(cook + 0x38, CARD_ALLOC_END);
    wr32(cook + 0x3C, 0);
    wr32(cook + 0x40, 1023);
    wr32(cook + 0x44, 1022);
    for (i = 0; i < 32; i++)
        wr32(cook + 0x50 + 4 * i, i == 0 ? CARD_FIRST_IFC : 0xFFFFFFFFu);
    for (i = 0; i < 32; i++)
        wr32(cook + 0xD0 + 4 * i, 0xFFFFFFFFu);
    cook[0x150] = 2;
    cook[0x151] = 0x2B;
    /* indirect FAT cluster and the FAT clusters it lists */
    ifc = cook + (size_t)CARD_FIRST_IFC * 1024;
    for (i = 0; i < 256; i++)
        wr32(ifc + 4 * i, i < CARD_FAT_CLUSTERS ? CARD_FIRST_IFC + 1 + i : 0xFFFFFFFFu);

    /* lay out: root (2 clusters), save directory, files; each a contiguous chain */
    root_cl = next;
    next += root_cls;
    dir_cl = next;
    next += dir_cls;
    root = cook + (size_t)(CARD_ALLOC_OFF + root_cl) * 1024;
    dir = cook + (size_t)(CARD_ALLOC_OFF + dir_cl) * 1024;
#define CHAIN(first, count)                                                    \
    do {                                                                       \
        uint32_t q;                                                            \
        for (q = 0; q < (count); q++)                                          \
            fat[(first) + q] = q + 1 < (count) ? ((first) + q + 1) | 0x80000000u : 0xFFFFFFFFu; \
    } while (0)
    CHAIN(root_cl, root_cls);
    CHAIN(dir_cl, dir_cls);
    put_ent(root, MODE_DIR, 3, now, now, 0, 0, ".");
    put_ent(root + 512, 6 | DF_DIR | DF_0400 | DF_HIDDEN | DF_EXISTS, 0, now, now, 0, 0, "..");
    put_ent(root + 1024, s->mode ? s->mode : MODE_DIR, dir_ents, s->ctime, s->mtime, dir_cl, 0, s->dir);
    put_ent(dir, MODE_DIR, 0, s->ctime, s->ctime, root_cl, 2, ".");
    put_ent(dir + 512, MODE_DIR, 0, s->ctime, s->ctime, 0, 0, "..");
    for (k = 0; k < s->nfiles; k++) {
        const Ps2sFile *f = &s->files[k];
        uint32_t cls = (f->size + 1023) / 1024, first = cls ? next : 0xFFFFFFFFu;
        put_ent(dir + 512 * (size_t)(k + 2), f->mode ? f->mode : MODE_FILE, f->size, f->ctime, f->mtime, first, 0, f->name);
        if (cls) {
            CHAIN(next, cls);
            memcpy(cook + (size_t)(CARD_ALLOC_OFF + next) * 1024, f->data, f->size);
            next += cls;
        }
    }
#undef CHAIN
    for (i = 0; i < CARD_FAT_CLUSTERS; i++)
        for (c = 0; c < 256; c++)
            wr32(cook + (size_t)(CARD_FIRST_IFC + 1 + i) * 1024 + 4 * c, fat[i * 256 + c]);
    /* the spare erase block (good_block2) is erased */
    memset(cook + (size_t)1022 * 16 * 512, 0xFF, 16 * 512);
    (void)zero;

    /* raw image: 512 data + 16 spare (3 ECC bytes per 128) per page */
    img = malloc((size_t)16384 * 528);
    if (!img) {
        free(cook); free(fat);
        return fail(err, en, "out of memory");
    }
    for (p = 0; p < 16384; p++) {
        uint8_t *d = img + p * 528;
        memcpy(d, cook + p * 512, 512);
        if (p / 16 == 1022)
            memset(d + 512, 0xFF, 16);
        else {
            memset(d + 512, 0, 16);
            for (k = 0; k < 4; k++)
                ecc128(d + 128 * k, d + 512 + 3 * k);
        }
    }
    free(cook);
    free(fat);
    *out = img;
    *outn = (size_t)16384 * 528;
    return 0;
}

/* ------------------------------------------------------------------ front doors */
int ps2s_read(const uint8_t *b, size_t n, const char *want, Ps2sSave *o, char *err, size_t en)
{
    int f = ps2s_detect(b, n), r;
    memset(o, 0, sizeof *o);
    if (err && en)
        err[0] = 0;
    switch (f) {
    case PS2S_PSU: r = read_psu(b, n, o, err, en); break;
    case PS2S_MAX: r = read_max(b, n, o, err, en); break;
    case PS2S_CBS: r = read_cbs(b, n, o, err, en); break;
    case PS2S_SPS: r = read_sps(b, n, o, err, en); break;
    case PS2S_CARD: r = read_card(b, n, want, o, err, en); break;
    default:
        return fail(err, en, "unrecognised save format (not .psu, .max, .cbs, .sps/.xps or a memory card image)");
    }
    if (r)
        ps2s_free(o);
    return r;
}

int ps2s_write(int fmt, const Ps2sSave *s, uint8_t **out, size_t *outn, char *err, size_t en)
{
    *out = NULL;
    *outn = 0;
    switch (fmt) {
    case PS2S_PSU: return write_psu(s, out, outn) ? fail(err, en, "out of memory") : 0;
    case PS2S_MAX: return write_max(s, out, outn, err, en);
    case PS2S_CBS: return write_cbs(s, out, outn, err, en);
    case PS2S_SPS: return write_sps(s, out, outn, err, en);
    case PS2S_CARD: return write_card(s, out, outn, err, en);
    }
    return fail(err, en, "unknown output format");
}

/* decode_data (main 0x2814E0 area, src/main/mc/mccomb.c): u16 version 0x100, key seed, stored
 * sum, 0x5963, then 0x8A20 words XORed with key = key * 0xB0 % 65363 (key 0 restarts at 1); the
 * stored sum is the 16-bit sum of the plain words. */
int ps2s_check_mh1_data(const uint8_t *d, size_t n, const char **why)
{
    unsigned key, stored, sum = 0, i;
    if (n < 0x11448) {
        *why = "file is too short for a Monster Hunter save";
        return -1;
    }
    if (rd16(d) != 0x100) {
        *why = "not a Monster Hunter save (version word is not 0x100)";
        return -1;
    }
    key = rd16(d + 2);
    stored = rd16(d + 4);
    for (i = 0; i < 0x8A20; i++) {
        sum = (sum + (rd16(d + 8 + 2 * i) ^ key)) & 0xFFFF;
        if (key == 0)
            key = 1;
        key = key * 0xB0 % 65363 & 0xFFFF;
    }
    if (sum != stored) {
        *why = "the save's own checksum does not match (damaged or a different game)";
        return -1;
    }
    return 0;
}
