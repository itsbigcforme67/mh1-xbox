/*
 * rt_data.c - game data tables used by the decompiled C the port runs.
 *
 * Capcom's tables are not copied into the repo: each one is declared here
 * empty and filled at start-up from the user's own SLPM_654.95 / game.bin
 * (rt_import_data). Addresses are from config/symbols/main.txt and
 * game.txt. PS2 and x86 are both little-endian, so tables are copied byte
 * for byte; then every pointer word in them (the ELF's R_MIPS_32
 * relocations) is turned into a host pointer: to the host copy of a table
 * if it points into one, else to the host symbol of that name (functions,
 * work areas; found with dlsym, the binary is linked -rdynamic), else to
 * the same bytes in the loaded image (which is relocated the same way).
 */
#define _GNU_SOURCE 1   /* dlsym RTLD_DEFAULT */
#include "rt.h"
#include "types.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
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

/* eft01 (lobby.bin enemy_shadow_size_lb, D_610300): the lobby overlay is
 * not loaded; zeros */
f32 D_610300[0x46][2];

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

/* the generated list (build/pc/rt_tables.c from tables.txt) */
struct rt_table { const char *name; uint32_t va; void *dst; size_t size; };
extern const struct rt_table rt_auto_tables[];
/* data the linked overlay C needs that nothing else defines
 * (build/pc/rt_gen.c from tools/gen_rt_auto.py): main and lobby.bin */
extern const struct rt_table rt_gen_main_tables[];

/* PS2 address -> host pointer (see the header comment) */
static int map_tables;     /* 1 while relocating the host tables (for RT_TRACE) */
static void *map_ptr(uint32_t v)
{
    const struct rt_table *t;
    size_t i;
    uint32_t off;
    int func = 0;
    const char *name;
    void *h;
    for (t = rt_auto_tables; t->name; t++)
        if (v >= t->va && v < t->va + t->size)
            return (uint8_t *)t->dst + (v - t->va);
    for (t = rt_gen_main_tables; t->name; t++)
        if (v >= t->va && v < t->va + t->size)
            return (uint8_t *)t->dst + (v - t->va);
    for (i = 0; i < sizeof tables / sizeof tables[0]; i++)
        if (v >= tables[i].va && v < tables[i].va + tables[i].size)
            return (uint8_t *)tables[i].dst + (v - tables[i].va);
    name = rt_sym_at(v, &off, &func);
    if (name && (h = dlsym(RTLD_DEFAULT, name)) != NULL)
        return (uint8_t *)h + off;
    if (func) {         /* code that is not ported: leave no MIPS address behind */
        if (map_tables && getenv("RT_TRACE"))
            fprintf(stderr, "rt: pointer to unported function %s+0x%X\n", name, (unsigned)off);
        return NULL;
    }
    if (rt_in_bss(v, 1))
        return rt_bss_shadow(v);
    return (void *)rt_addr(v, 1);
}

const void *rt_ptr_at(uint32_t va)
{
    const uint8_t *p = rt_addr(va, 4);
    uint32_t h;
    if (!p || !rt_is_pointer(va))
        return NULL;
    memcpy(&h, p, 4);
    return (const void *)(uintptr_t)h;
}

static int import_list(const struct rt_table *t)
{
    int missing = 0;
    for (; t->name; t++) {
        const uint8_t *p = rt_addr(t->va, t->size);
        if (p) {
            memcpy(t->dst, p, t->size);
        } else if (rt_in_bss(t->va, t->size)) {
            memset(t->dst, 0, t->size);
        } else {
            fprintf(stderr, "rt: data table %s (0x%X) not found\n", t->name, (unsigned)t->va);
            missing++;
        }
    }
    return missing;
}

int rt_import_data(void)
{
    size_t i;
    int k, missing = 0;
    const struct rt_table *t;
    missing += import_list(rt_gen_main_tables);
    for (t = rt_auto_tables; t->name; t++) {
        const uint8_t *p = rt_addr(t->va, t->size);
        if (p) {
            memcpy(t->dst, p, t->size);
        } else if (rt_in_bss(t->va, t->size)) {
            memset(t->dst, 0, t->size);
        } else {
            fprintf(stderr, "rt: data table %s (0x%X) not found\n", t->name, (unsigned)t->va);
            missing++;
        }
    }
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
    /* pointers: host copies first, then the images themselves */
    if (rt_load_relocs() == 0) {
        map_tables = 1;
        for (t = rt_auto_tables; t->name; t++)
            rt_relocate_range(t->va, t->dst, t->size, map_ptr);
        for (t = rt_gen_main_tables; t->name; t++)
            rt_relocate_range(t->va, t->dst, t->size, map_ptr);
        for (i = 0; i < sizeof tables / sizeof tables[0]; i++)
            rt_relocate_range(tables[i].va, tables[i].dst, tables[i].size, map_ptr);
        map_tables = 0;
        rt_relocate_images(map_ptr);
    } else {
        fprintf(stderr, "rt: no relocations in the ELF: pointers in data tables stay PS2 addresses\n");
    }
    return missing;
}

/* ------------------------------------------------------------ lobby.bin
 * The lobby/village overlay's data: one host block, rt_lb_mem, holds the
 * whole overlay (image + .bss) at its PS2 layout, and every lobby data
 * symbol the C uses is a linker alias into it (tools/gen_rt_auto.py), so a
 * table read past its end sees its PS2 neighbours. rt_import_lobby copies
 * the image in and turns its pointer words (.rellobby.bin) into host
 * pointers: lobby functions to the host function of that name (dlsym),
 * lobby data into rt_lb_mem, main addresses as for the ELF (map_ptr).
 * Call after rt_import_data. */
#define LB_VRAM 0x533980u
#define LB_SPAN 0x220000u       /* lobby.bin 0x134E00 + .bss 0xEA680, rounded up */
uint8_t rt_lb_mem[LB_SPAN] __attribute__((aligned(16)));
static uint8_t *lb_image;       /* rt_lb_mem as rt_import_lobby left it */
static uint32_t lb_image_n;

static void *map_lb(uint32_t v)
{
    uint32_t off;
    int func = 0;
    const char *name;
    void *h;
    if (!rt_lb_in_range(v))
        return map_ptr(v);
    name = rt_lb_sym_at(v, &off, &func);
    if (func && name) {
        if ((h = dlsym(RTLD_DEFAULT, name)) != NULL)
            return (uint8_t *)h + off;
        {   /* em10_local_init_0053DCE0: the C has the plain name */
            size_t n = strlen(name);
            char plain[128];
            if (n > 9 && n < sizeof plain && name[n - 9] == '_' && strspn(name + n - 8, "0123456789ABCDEF") == 8) {
                memcpy(plain, name, n - 9);
                plain[n - 9] = 0;
                if ((h = dlsym(RTLD_DEFAULT, plain)) != NULL)
                    return (uint8_t *)h + off;
            }
        }
        {   /* the other way round: the map has the plain name of a file
             * static whose C carries the address suffix (em10_local_init) */
            char sfx[160];
            snprintf(sfx, sizeof sfx, "%s_%08X", name, (unsigned)(v - off));
            if ((h = dlsym(RTLD_DEFAULT, sfx)) != NULL)
                return (uint8_t *)h + off;
        }
        if (getenv("RT_TRACE"))
            fprintf(stderr, "rt: lobby pointer to unported function %s+0x%X\n", name, (unsigned)off);
        return NULL;
    }
    return v - LB_VRAM < LB_SPAN ? rt_lb_mem + (v - LB_VRAM) : NULL;
}

int rt_import_lobby(void)
{
    const uint8_t *img = rt_lb_addr(LB_VRAM, 4);
    uint32_t n = 0;
    if (!img)
        return 1;
    while (n + 0x1000 <= LB_SPAN && rt_lb_addr(LB_VRAM + n, 0x1000))
        n += 0x1000;
    while (n < LB_SPAN && rt_lb_addr(LB_VRAM + n, 1))
        n++;
    memcpy(rt_lb_mem, img, n);
    memset(rt_lb_mem + n, 0, LB_SPAN - n);
    rt_lb_relocate_range(LB_VRAM, rt_lb_mem, n, map_lb);
    {   /* main's data words that point into lobby.bin (the ELF's .relmain
         * names their symbol's section): game.bin shares that vram, so
         * rt_import_data sent them into game.bin. pit_help_str_tbl[4]/[5]
         * (village menu help), shop_default_tag (item shop "buy"/"sell"),
         * my_job_str (the forge's "blademaster"/"gunner" title), hint_tbl[0]
         * (spot hints), armor_shop_tblA/B, plaza menus, ... 72 words. */
        const uint32_t *pr;
        size_t k, np = rt_main_lobby_ptrs(&pr);
        for (k = 0; k < np; k++) {
            uint8_t *dst = map_ptr(pr[2 * k]);
            void *h = map_lb(pr[2 * k + 1]);
            if (dst)
                memcpy(dst, &h, sizeof h);
        }
    }
    lb_image_n = n;
    lb_image = malloc(n);       /* the freshly loaded overlay, for rt_lb_reload */
    if (lb_image)
        memcpy(lb_image, rt_lb_mem, n);
    return 0;
}

/* Load_overlay(3) (main 0x23E510: load_bin + mwOverlayInit) on every entry
 * to the village: lobby.bin's data comes back as on disc and its .bss is
 * zeroed (game.bin used the same memory during the quest). Without it
 * client_work kept "village motions loaded" (cw+0x2C06) from the last
 * visit while the quest had replaced them: the hunter walked on the spot. */
void rt_lb_reload(void)
{
    if (!lb_image)
        return;
    memcpy(rt_lb_mem, lb_image, lb_image_n);
    memset(rt_lb_mem + lb_image_n, 0, LB_SPAN - lb_image_n);
}

/* ------------------------------------------------------------ select.bin
 * The boot overlay's data (tables and strings; it has no .bss): one host
 * block, rt_sel_mem, at its PS2 layout; its data symbols are linker
 * aliases into it (tools/gen_rt_auto.py, config/symbols/select.txt).
 * Pointer words (.relselect.bin): select functions to the host function
 * of that name, select data into rt_sel_mem, main as for the ELF. */
#define SEL_SPAN 0x8000u        /* select.bin is 0x8000 bytes */
uint8_t rt_sel_mem[SEL_SPAN] __attribute__((aligned(16)));

static void *map_sel(uint32_t v)
{
    uint32_t off;
    int func = 0;
    const char *name;
    void *h;
    if (!rt_sel_in_range(v))
        return map_ptr(v);
    name = rt_sel_sym_at(v, &off, &func);
    if (func && name) {
        if ((h = dlsym(RTLD_DEFAULT, name)) != NULL)
            return (uint8_t *)h + off;
        if (getenv("RT_TRACE"))
            fprintf(stderr, "rt: select pointer to unported function %s+0x%X\n", name, (unsigned)off);
        return NULL;
    }
    return v - LB_VRAM < SEL_SPAN ? rt_sel_mem + (v - LB_VRAM) : NULL;
}

int rt_import_select(void)
{
    uint32_t n = 0;
    if (!rt_sel_addr(LB_VRAM, 4))
        return 1;
    while (n < SEL_SPAN && rt_sel_addr(LB_VRAM + n, 1))
        n++;
    memcpy(rt_sel_mem, rt_sel_addr(LB_VRAM, n), n);
    memset(rt_sel_mem + n, 0, SEL_SPAN - n);
    rt_sel_relocate_range(LB_VRAM, rt_sel_mem, n, map_sel);
    return 0;
}
