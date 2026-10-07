/*
 * rt_memstat.c - live heap bytes per category for the port's own C (see
 * rt_memstat.h). A pointer -> (size, category) table; frees of pointers it
 * does not know (allocated by a library) pass straight through.
 */
#define RT_MEMSTAT_IMPL
#include "rt_memstat.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define NCAT 48
static struct { const char *name; long cur, peak; } cats[NCAT];
static const char *ctx;

#define HBITS 20
#define HN (1u << HBITS)
static struct { void *p; uint32_t n; uint8_t cat; } *tab;
static int enabled = -1;

static int on(void)
{
    if (enabled < 0) {
        enabled = getenv("RT_MEM") != NULL;
        if (enabled && !(tab = calloc(HN, sizeof *tab)))
            enabled = 0;
    }
    return enabled;
}

static int cat_of(const char *file)
{
    static const struct { const char *pat, *cat; } rules[] = {
        { "fl_model", "models (host clays, skinning)" }, { "fmt/amo", "models (host clays, skinning)" },
        { "fmt/ahi", "motions/skeletons" }, { "fmt/aan", "motions/skeletons" }, { "rt_motion", "motions/skeletons" },
        { "gfx_gl", "renderer CPU-side (vertex arrays)" }, { "gfx_rec", "renderer CPU-side (vertex arrays)" },
        { "fmt/apx", "texture decode buffers" },
        { "fmt/snd", "audio decoded PCM" }, { "rt_snd", "audio tables" }, { "audio_", "audio tables" },
        { "rt_mem", "overlay data copies + relocations" }, { "fmt/afs", "AFS index / file reads" },
        { "fmt/melt", "file decompression" }, { "fmt/hits", "collision (host)" },
        { "viewer", "front-end (viewer.c)" },
    };
    const char *c = ctx;
    int i;
    if (!c) {
        c = strstr(file, "rt/rt_") ? file : "other";     /* the runtime: by file */
        for (i = 0; i < (int)(sizeof rules / sizeof rules[0]); i++)
            if (strstr(file, rules[i].pat)) {
                c = rules[i].cat;
                break;
            }
    }
    for (i = 0; i < NCAT && cats[i].name; i++)
        if (cats[i].name == c || !strcmp(cats[i].name, c))
            return i;
    if (i == NCAT)
        return NCAT - 1;
    cats[i].name = c;
    return i;
}

static void track(void *p, size_t n, int c)
{
    uint32_t h = (uint32_t)(((uintptr_t)p >> 3) * 2654435761u) >> (32 - HBITS);
    while (tab[h].p)
        h = (h + 1) & (HN - 1);
    tab[h].p = p;
    tab[h].n = (uint32_t)n;
    tab[h].cat = (uint8_t)c;
    cats[c].cur += (long)n;
    if (cats[c].cur > cats[c].peak)
        cats[c].peak = cats[c].cur;
}

static int untrack(void *p)        /* returns the category or -1 */
{
    uint32_t h = (uint32_t)(((uintptr_t)p >> 3) * 2654435761u) >> (32 - HBITS), j, k;
    int c;
    while (tab[h].p && tab[h].p != p)
        h = (h + 1) & (HN - 1);
    if (!tab[h].p)
        return -1;
    c = tab[h].cat;
    cats[c].cur -= (long)tab[h].n;
    tab[h].p = NULL;
    for (j = (h + 1) & (HN - 1); tab[j].p; j = (j + 1) & (HN - 1)) {   /* re-insert the cluster after the hole */
        k = (uint32_t)(((uintptr_t)tab[j].p >> 3) * 2654435761u) >> (32 - HBITS);
        if ((j > h && (k <= h || k > j)) || (j < h && (k <= h && k > j))) {
            tab[h] = tab[j];
            tab[j].p = NULL;
            h = j;
        }
    }
    return c;
}

void *rt_ms_malloc(size_t n, const char *file)
{
    void *p = malloc(n);
    if (p && on())
        track(p, n, cat_of(file));
    return p;
}
void *rt_ms_calloc(size_t k, size_t n, const char *file)
{
    void *p = calloc(k, n);
    if (p && on())
        track(p, k * n, cat_of(file));
    return p;
}
void *rt_ms_realloc(void *p, size_t n, const char *file)
{
    int c = p && on() ? untrack(p) : -1;
    void *q = realloc(p, n);
    if (on()) {
        if (q)
            track(q, n, c >= 0 ? c : cat_of(file));
    }
    return q;
}
void rt_ms_free(void *p)
{
    if (p && on())
        untrack(p);
    free(p);
}
const char *rt_ms_push(const char *cat) { const char *o = ctx; ctx = cat; return o; }
void rt_ms_pop(const char *prev) { ctx = prev; }
void rt_ms_add(const char *cat, long bytes)
{
    int c;
    const char *o;
    if (!on())
        return;
    o = ctx;
    ctx = cat;
    c = cat_of("");
    ctx = o;
    cats[c].cur += bytes;
    if (cats[c].cur > cats[c].peak)
        cats[c].peak = cats[c].cur;
}

void rt_ms_report(const char *where)
{
    long tot = 0, vm = 0, rss = 0;
    int i;
    FILE *f;
    char line[256];
    if (!on())
        return;
    fprintf(stderr, "memstat: --- %s (KB now / peak) ---\n", where);
    for (i = 0; i < NCAT && cats[i].name; i++) {
        fprintf(stderr, "memstat: %-38s %8ld %8ld\n", cats[i].name, cats[i].cur / 1024, cats[i].peak / 1024);
        if (!strstr(cats[i].name, "GPU)"))
            tot += cats[i].cur;
    }
    if ((f = fopen("/proc/self/status", "r"))) {
        while (fgets(line, sizeof line, f)) {
            sscanf(line, "VmRSS: %ld", &rss);
            sscanf(line, "VmData: %ld", &vm);
        }
        fclose(f);
    }
    fprintf(stderr, "memstat: %-38s %8ld\n", "tracked heap total (CPU)", tot / 1024);
    fprintf(stderr, "memstat: %-38s %8ld   (whole process incl. SDL/GL driver, libc, binary)\n", "process RSS", rss);
}
