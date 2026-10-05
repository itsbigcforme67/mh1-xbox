/*
 * rt_data.c - game data tables used by the decompiled C the port runs.
 *
 * Capcom's tables are not copied into the repo: each one is declared here
 * empty and filled at start-up from the user's own SLPM_654.95 / game.bin
 * (rt_import_data). Addresses are from config/symbols/main.txt and
 * game.txt. PS2 and x86 are both little-endian and these tables hold only
 * numbers (no pointers), so they are copied byte for byte.
 */
#include "rt.h"
#include "types.h"

#include <stdio.h>
#include <string.h>

/* set14 (src/game/set/set14_nm.c) */
s16 st00_mdl_tbl[4], st01_mdl_tbl[4], st04_mdl_tbl[4];
s16 set14_st00_mask_tbl[4], set14_st01_mask_tbl[4], set14_st03_mask_tbl[4], set14_st04_mask_tbl[4];
s16 set14_st42_mask_tbl[4], set14_st51_mask_tbl[4], set14_st52_mask_tbl[4], set14_st53_mask_tbl[4];
f32 set14_st42_pos_tbl[4][3];   /* the symbol is 0x24 bytes: only 3 rows are used (i 1..3) */
f32 set14_st51_pos_tbl[5][3], set14_st52_pos_tbl[5][3], set14_st53_pos_tbl[5][3];
f32 uv_pos00_00678370[16][2];

/* main: blend/filter tables (clay_attr_set, SetTrnslMode, ... in rt_fl.c) */
u32 src_mode_00300620[12], dst_mode_00300650[10], ope_mode_00300678[4], filter_mode_00387900[2];
u32 aa_alpha_src[10], aa_alpha_ope[4], aa_filt[2], aa_addr[4];

static const struct {
    const char *name;
    uint32_t va;
    void *dst;
    size_t size;
} tables[] = {
#define T(sym, va, size) { #sym, va, sym, size }
    T(src_mode_00300620, 0x300620, 0x30),
    T(dst_mode_00300650, 0x300650, 0x28),
    T(ope_mode_00300678, 0x300678, 0x10),
    T(filter_mode_00387900, 0x387900, 8),
    T(aa_alpha_src, 0x2EF710, 0x28),
    T(aa_alpha_ope, 0x2EF738, 0x10),
    T(aa_filt, 0x3876F8, 8),
    T(aa_addr, 0x2EF748, 0x10),
    T(st00_mdl_tbl, 0x389AC8, 8),
    T(st01_mdl_tbl, 0x389AD0, 8),
    T(st04_mdl_tbl, 0x389AD8, 8),
    T(set14_st00_mask_tbl, 0x389AE0, 8),
    T(set14_st01_mask_tbl, 0x389AE8, 8),
    T(set14_st03_mask_tbl, 0x389AF0, 8),
    T(set14_st04_mask_tbl, 0x389AF8, 8),
    T(set14_st42_mask_tbl, 0x389B00, 8),
    T(set14_st51_mask_tbl, 0x389B08, 8),
    T(set14_st52_mask_tbl, 0x389B10, 8),
    T(set14_st53_mask_tbl, 0x389B18, 8),
    T(uv_pos00_00678370, 0x678370, 0x80),
    T(set14_st42_pos_tbl, 0x6783F0, 0x24),
    T(set14_st51_pos_tbl, 0x678420, 0x3C),
    T(set14_st52_pos_tbl, 0x678460, 0x3C),
    T(set14_st53_pos_tbl, 0x6784A0, 0x3C),
#undef T
};

int rt_import_data(void)
{
    size_t i;
    int missing = 0;
    for (i = 0; i < sizeof tables / sizeof tables[0]; i++) {
        const uint8_t *p = rt_addr(tables[i].va, tables[i].size);
        if (p) {
            memcpy(tables[i].dst, p, tables[i].size);
        } else {
            fprintf(stderr, "rt: data table %s (0x%X) not found\n", tables[i].name, (unsigned)tables[i].va);
            missing++;
        }
    }
    return missing;
}
