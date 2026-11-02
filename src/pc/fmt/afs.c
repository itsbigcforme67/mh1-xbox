/* afs.c - AFS archive reader (format: tools/afs_extract.py). */
#include "fmt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* base: where the archive starts in the file (MH2's DATA.BIN holds two archives, fmt_afs_open_sub) */
static int afs_open_at(fmt_afs *a, const char *path, long base)
{
    FILE *f = fopen(path, "rb");
    uint8_t head[8];
    long fsize;
    uint32_t i, first = 0xFFFFFFFFu;
    long ptrpos[2];
    int k;

    memset(a, 0, sizeof *a);
    if (!f)
        return -1;
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, base, SEEK_SET);
    if (fread(head, 1, 8, f) != 8 || memcmp(head, "AFS", 3) != 0) {
        fclose(f);
        return -1;
    }
    a->fp = f;
    a->count = fmt_u32(head + 4, FMT_LE);
    a->off = calloc(a->count, 4);
    a->size = calloc(a->count, 4);
    a->name = calloc(a->count, 32);
    for (i = 0; i < a->count; i++) {
        uint8_t e[8];
        if (fread(e, 1, 8, f) != 8)
            return -1;
        a->off[i] = fmt_u32(e, FMT_LE);
        a->size[i] = fmt_u32(e + 4, FMT_LE);
        if (a->off[i] && a->off[i] < first)
            first = a->off[i];
    }
    /* name directory pointer: after the table, or just before the first file */
    ptrpos[0] = 8 + (long)a->count * 8;
    ptrpos[1] = (long)first - 8;
    for (k = 0; k < 2; k++) {
        uint8_t p[8];
        uint32_t doff, dsize;
        if (ptrpos[k] < 8 || base + ptrpos[k] + 8 > fsize)
            continue;
        fseek(f, base + ptrpos[k], SEEK_SET);
        if (fread(p, 1, 8, f) != 8)
            continue;
        doff = fmt_u32(p, FMT_LE);
        dsize = fmt_u32(p + 4, FMT_LE);
        if (doff && dsize >= a->count * 0x30 && base + (long)doff + (long)dsize <= fsize) {
            uint8_t rec[0x30];
            fseek(f, base + doff, SEEK_SET);
            for (i = 0; i < a->count; i++) {
                if (fread(rec, 1, 0x30, f) != 0x30)
                    break;
                memcpy(a->name[i], rec, 31);
                a->name[i][31] = 0;
            }
            break;
        }
    }
    for (i = 0; i < a->count; i++)
        if (a->off[i])
            a->off[i] += (uint32_t)base;
    return 0;
}

int fmt_afs_open(fmt_afs *a, const char *path)
{
    return afs_open_at(a, path, 0);
}

/* An archive stored as entry `entry` of the archive at path (MH2's DATA.BIN: entry 0 the data files, entry 1 the overlays) */
int fmt_afs_open_sub(fmt_afs *a, const char *path, int entry)
{
    fmt_afs top;
    long base;
    if (fmt_afs_open(&top, path) != 0)
        return -1;
    if (entry < 0 || (uint32_t)entry >= top.count) {
        fmt_afs_close(&top);
        return -1;
    }
    base = (long)top.off[entry];
    fmt_afs_close(&top);
    return afs_open_at(a, path, base);
}

/* An overlay by the name in its MWo3 header (+0x20): MH2's overlay archive has no name directory; -1 if absent */
int fmt_afs_find_mwo(const fmt_afs *a, const char *name)
{
    uint32_t i;
    for (i = 0; i < a->count; i++) {
        uint8_t h[0x40];
        if (fmt_afs_read_at(a, (int)i, 0, h, sizeof h) == sizeof h && !memcmp(h, "MWo3", 4) && !strcmp((const char *)h + 0x20, name))
            return (int)i;
    }
    return -1;
}

void fmt_afs_close(fmt_afs *a)
{
    if (a->fp)
        fclose(a->fp);
    free(a->off);
    free(a->size);
    free(a->name);
    memset(a, 0, sizeof *a);
}

int fmt_afs_find(const fmt_afs *a, const char *name)
{
    uint32_t i;
    for (i = 0; i < a->count; i++)
        if (strcmp(a->name[i], name) == 0)
            return (int)i;
    return -1;
}

void (*fmt_time_hook)(int zone, int begin, long bytes);

uint8_t *fmt_afs_read(const fmt_afs *a, int idx, size_t *len)
{
    uint8_t *buf;
    size_t got;
    if (idx < 0 || (uint32_t)idx >= a->count)
        return NULL;
    buf = malloc(a->size[idx] ? a->size[idx] : 1);
    if (fmt_time_hook)
        fmt_time_hook(FMT_T_READ, 1, 0);
    fseek(a->fp, a->off[idx], SEEK_SET);
    got = fread(buf, 1, a->size[idx], a->fp);
    if (fmt_time_hook)
        fmt_time_hook(FMT_T_READ, 0, (long)got);
    if (got != a->size[idx]) {
        free(buf);
        return NULL;
    }
    *len = a->size[idx];
    return buf;
}

uint8_t *fmt_afs_load(const fmt_afs *a, const char *name, size_t *len)
{
    size_t rawlen;
    uint8_t *raw = fmt_afs_read(a, fmt_afs_find(a, name), &rawlen);
    uint8_t *out;
    if (!raw)
        return NULL;
    out = fmt_melt(raw, rawlen, len, FMT_LE);
    if (!out && a->raw_ok) {    /* MH2 stores some files as they are (village.bin, plcom_tbl.bin, ...): what is not a Meltw stream is raw */
        *len = rawlen;
        return raw;
    }
    free(raw);
    return out;
}

size_t fmt_afs_read_at(const fmt_afs *a, int idx, uint32_t off, void *buf, size_t n)
{
    if (idx < 0 || (uint32_t)idx >= a->count || off >= a->size[idx])
        return 0;
    if (n > a->size[idx] - off)
        n = a->size[idx] - off;
    if (fmt_time_hook)
        fmt_time_hook(FMT_T_READ, 1, 0);
    fseek(a->fp, (long)a->off[idx] + (long)off, SEEK_SET);
    n = fread(buf, 1, n, a->fp);
    if (fmt_time_hook)
        fmt_time_hook(FMT_T_READ, 0, (long)n);
    return n;
}
