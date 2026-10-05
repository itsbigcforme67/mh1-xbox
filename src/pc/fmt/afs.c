/* afs.c - AFS archive reader (format: tools/afs_extract.py). */
#include "fmt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fmt_afs_open(fmt_afs *a, const char *path)
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
    fseek(f, 0, SEEK_SET);
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
        if (ptrpos[k] < 8 || ptrpos[k] + 8 > fsize)
            continue;
        fseek(f, ptrpos[k], SEEK_SET);
        if (fread(p, 1, 8, f) != 8)
            continue;
        doff = fmt_u32(p, FMT_LE);
        dsize = fmt_u32(p + 4, FMT_LE);
        if (doff && dsize >= a->count * 0x30 && (long)doff + (long)dsize <= fsize) {
            uint8_t rec[0x30];
            fseek(f, doff, SEEK_SET);
            for (i = 0; i < a->count; i++) {
                if (fread(rec, 1, 0x30, f) != 0x30)
                    break;
                memcpy(a->name[i], rec, 31);
                a->name[i][31] = 0;
            }
            break;
        }
    }
    return 0;
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

uint8_t *fmt_afs_read(const fmt_afs *a, int idx, size_t *len)
{
    uint8_t *buf;
    if (idx < 0 || (uint32_t)idx >= a->count)
        return NULL;
    buf = malloc(a->size[idx] ? a->size[idx] : 1);
    fseek(a->fp, a->off[idx], SEEK_SET);
    if (fread(buf, 1, a->size[idx], a->fp) != a->size[idx]) {
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
    free(raw);
    return out;
}

size_t fmt_afs_read_at(const fmt_afs *a, int idx, uint32_t off, void *buf, size_t n)
{
    if (idx < 0 || (uint32_t)idx >= a->count || off >= a->size[idx])
        return 0;
    if (n > a->size[idx] - off)
        n = a->size[idx] - off;
    fseek(a->fp, (long)a->off[idx] + (long)off, SEEK_SET);
    return fread(buf, 1, n, a->fp);
}
