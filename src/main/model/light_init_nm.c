/* light_init_nm - SLPM_654.95 0x0011DB10-0x0011DD58: light_init (near-match copy for the PC; the PS2 build links the original bytes, -O4 inlines the helper). Clears light_work (two sets of 0x140 bytes) and fills the three lights of
 * each set from the stage light rows: set 0 from stg_light_tbl (the same for every stage; the colour rows are scaled by 10), set 1 from
 * pl_light_tbl[stage]. Light block i of a set is at set + 0x10 + 8 + i * 0x68 (the LGT struct flSetRenderState(0x5A + i) copies):
 *   +0x04 colour (rgb), +0x14 second colour row, +0x24 third colour row (the ambient part), +0x34 direction, +0x40 fourth row.
 * Tables: [0] fourth rows and [1] directions (3 floats per light), [2] colours, [3] ambient, [4] second rows (4 floats per light). */
#include "types.h"
#include "game.h"

extern GAME_W game_w;
extern u8 light_work[];
extern u8 *stg_light_tbl[5];
extern u8 pl_light_tbl[];       /* per stage 0x14 bytes: five row pointers */
void *memset(void *, int, u32);

static void light_fill(u8 *b, u8 **tbl, f32 k)
{
    int i;
    int o12 = 0;
    int o16 = 0;
    f32 *s;

    for (i = 0; i < 3; i++) {
        s = (f32 *)(tbl[0] + o12);
        *(f32 *)(b + 0x48) = s[0];
        *(f32 *)(b + 0x4C) = s[1];
        *(f32 *)(b + 0x50) = s[2];
        s = (f32 *)(tbl[1] + o12);
        o12 += 0xC;
        *(f32 *)(b + 0x3C) = s[0];
        *(f32 *)(b + 0x40) = s[1];
        *(f32 *)(b + 0x44) = s[2];
        s = (f32 *)(tbl[2] + o16);
        *(f32 *)(b + 0xC) = k * s[0];
        *(f32 *)(b + 0x10) = k * s[1];
        *(f32 *)(b + 0x14) = k * s[2];
        s = (f32 *)(tbl[3] + o16);
        *(f32 *)(b + 0x2C) = s[0];
        *(f32 *)(b + 0x30) = s[1];
        *(f32 *)(b + 0x34) = s[2];
        s = (f32 *)(tbl[4] + o16);
        o16 += 0x10;
        *(f32 *)(b + 0x1C) = s[0];
        *(f32 *)(b + 0x20) = s[1];
        *(f32 *)(b + 0x24) = s[2];
        *(s32 *)(b + 8) = 0;
        b += 0x68;
    }
}

void light_init(void)
{
    memset(light_work, 0, 0x290);
    light_fill(light_work + 0x10, stg_light_tbl, 10.0f);
    light_fill(light_work + 0x150, (u8 **)(pl_light_tbl + game_w.stage * 0x14), 1.0f);
}
