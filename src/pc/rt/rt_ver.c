/*
 * rt_ver.c - which PS2 game the port runs: Monster Hunter (MH1 JP, SLPM_654.95) or
 * Monster Hunter G (SLPM_658.69), and the address facts that differ between them
 * (docs/mhg.md section 4). The facts are generated per build (tools/gen_rt_meta.py ->
 * build/pc/rt_meta.c): for G its symbols and data pointer words (its ELF is stripped)
 * and a map from MH1 addresses to G addresses through names both have, for the places
 * in src/pc that still name an MH1 address (RTVA in rt.h).
 */
#include "rt.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct { uint32_t mh1, size, va; } rt_meta_va;
extern const int rt_meta_version;
extern const char rt_meta_elf[];
extern const rt_meta_va rt_meta_va_main[], rt_meta_va_lobby[], rt_meta_va_select[];
extern const size_t rt_meta_va_main_n, rt_meta_va_lobby_n, rt_meta_va_select_n;

int rt_ver_mhg(void) { return rt_meta_version >= 1; }     /* G, or MH2 (which runs G's form of the C) */
int rt_ver_mh2(void) { return rt_meta_version == 2; }
const char *rt_ver_elf(void) { return rt_meta_elf; }

uint32_t rt_va_mh1(int set, uint32_t mh1)
{
    const rt_meta_va *t;
    size_t n, lo = 0, hi, k;
    if (rt_meta_version == 0 || mh1 == 0)
        return mh1;
    t = set == 1 ? rt_meta_va_lobby : set == 2 ? rt_meta_va_select : rt_meta_va_main;
    n = set == 1 ? rt_meta_va_lobby_n : set == 2 ? rt_meta_va_select_n : rt_meta_va_main_n;
    hi = n;
    while (lo < hi) {                   /* last row with mh1 address <= mh1 */
        size_t mid = (lo + hi) / 2;
        if (t[mid].mh1 <= mh1)
            lo = mid + 1;
        else
            hi = mid;
    }
    for (k = lo; k-- > 0 && k + 64 >= lo;) {
        if (t[k].mh1 == mh1)
            return t[k].va;
        if (t[k].size && mh1 < t[k].mh1 + t[k].size)
            return t[k].va + (mh1 - t[k].mh1);
    }
    {
        static uint32_t warned[64];
        static int nw;
        int i;
        for (i = 0; i < nw; i++)
            if (warned[i] == mh1)
                return 0;
        if (nw < 64)
            warned[nw++] = mh1;
        rt_warn("G: no address for MH1 0x%X (set %d): not mapped yet", (unsigned)mh1, set);
    }
    return 0;
}
