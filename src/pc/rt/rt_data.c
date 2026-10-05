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

/* set00 (src/game/set/set00.c). The pos tables are read with an 8-byte
 * stride but 3 floats per entry (see set00.c), so they keep their full size. */
f32 set00_st04_pos_tbl[3][2], set00_st08_pos_tbl[9][2], set00_st26_pos_tbl[3][2];
f32 set00_st41_pos_tbl[2][2], set00_st42_pos_tbl[3][2], set00_st43_pos_tbl[3][2];
s16 set00_st04_dir_tbl[2], set00_st08_dir_tbl[6], set00_st26_dir_tbl[2];
s16 set00_st41_dir_tbl[1][2], set00_st42_dir_tbl[2], set00_st43_dir_tbl[2];
f32 set00_st26_scale_tbl[2], set00_st41_scale_tbl[1][3], set00_st42_scale_tbl[2];

/* set09 (src/game/set/set09.c) and the stage start positions (main) */
f32 set09_st08_beetle_tbl[4][3], set09_st05_type09_pos[16], set09_st33_bird_tbl[2][3];
u16 set09_st05_type09_ang[16];
f32 stage_start_pos[88][3];

/* set17 (src/game/set/set17.c) */
u8 st01_parts_id_tbl[4], st02_parts_id_tbl[20], st03_parts_id_tbl[12], st46_parts_id_tbl[30];

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
    T(set00_st04_pos_tbl, 0x678160, 0x18),
    T(set00_st08_pos_tbl, 0x678180, 0x48),
    T(set00_st08_dir_tbl, 0x6781C8, 0xC),
    T(set00_st26_pos_tbl, 0x6781E0, 0x18),
    T(set00_st41_pos_tbl, 0x6781F8, 0xC),
    T(set00_st41_scale_tbl, 0x678208, 0xC),
    T(set00_st42_pos_tbl, 0x678220, 0x18),
    T(set00_st43_pos_tbl, 0x678240, 0x18),
    T(set00_st04_dir_tbl, 0x389AA0, 4),
    T(set00_st26_dir_tbl, 0x389AA4, 4),
    T(set00_st26_scale_tbl, 0x389AA8, 8),
    T(set00_st41_dir_tbl, 0x389AB0, 4),
    T(set00_st42_dir_tbl, 0x389AB4, 4),
    T(set00_st42_scale_tbl, 0x389AB8, 8),
    T(set00_st43_dir_tbl, 0x389AC0, 4),
    T(set09_st08_beetle_tbl, 0x677A10, 0x30),
    T(set09_st05_type09_pos, 0x677A40, 0x40),
    T(set09_st05_type09_ang, 0x677A80, 0x20),
    T(set09_st33_bird_tbl, 0x677AA0, 0x18),
    T(stage_start_pos, 0x2F2620, 0x420),
    T(st01_parts_id_tbl, 0x389B20, 4),
    T(st02_parts_id_tbl, 0x6784F0, 0x14),
    T(st03_parts_id_tbl, 0x678508, 0xC),
    T(st46_parts_id_tbl, 0x678520, 0x1E),
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

/* Tables of pointers (one per stage) into the game's data. Each PS2 pointer
 * is translated to the same bytes in the loaded ELF/overlay image; the
 * game C only reads through them. */
f32 *sun_pos_tbl[88];          /* set13: sun position per stage */
s16 *stg_eft_mdl_no[88];       /* set13: set-model clay numbers per stage */
f32 *stage_sphr_tbl[88];       /* set13: occluding spheres r,x,y,z ... r = -1 */

static const struct {
    const char *name;
    uint32_t va;
    void **dst;
    int n;
} ptables[] = {
#define P(sym, va) { #sym, va, (void **)sym, (int)(sizeof sym / sizeof sym[0]) }
    P(sun_pos_tbl, 0x2F7160),
    P(stg_eft_mdl_no, 0x2F6C00),
    P(stage_sphr_tbl, 0x2F1CF0),
#undef P
};

int rt_import_data(void)
{
    size_t i;
    int k, missing = 0;
    for (i = 0; i < sizeof ptables / sizeof ptables[0]; i++) {
        const uint8_t *p = rt_addr(ptables[i].va, 4 * (size_t)ptables[i].n);
        if (!p) {
            fprintf(stderr, "rt: pointer table %s (0x%X) not found\n", ptables[i].name, (unsigned)ptables[i].va);
            missing++;
            continue;
        }
        for (k = 0; k < ptables[i].n; k++) {
            uint32_t va;
            memcpy(&va, p + 4 * k, 4);
            ptables[i].dst[k] = va ? (void *)rt_addr(va, 4) : NULL;
        }
    }
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
