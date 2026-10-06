/* light01 - light set / player light reset (SLPM_654.95 0x0011E4C0-0x0011E540): light_set, Pl_light_init. Whole file in light_nm.c. */
/* light_nm - SLPM_654.95 0x0011DB04-0x0011E9D8 (light_init.s) small parts: light_set, light_change_normal, Pl_light_init,
   release_tex_all; get_mdlw_ptr (0x00123C70). light_work: 2 sets of 0x140 bytes (3 lights of 0x68 starting at +0x18). Working file. */
#include "types.h"
#include "game.h"
extern GAME_W game_w;
extern u8 light_work[];
extern u8 pl_light_tbl[];
/* per stage 0x14 bytes: pointers to light colour/direction rows */
extern s32 mem_tex[];
extern u8 *mdlw_heap_area;
void flSetRenderState(int, int);
int flReleasePaletteHandle(u32);
int flReleaseTextureHandle(u16);
void light_set(int n) {
    u8 *p = light_work + n * 0x140 + 0x10;

    flSetRenderState(0x5A, (int)(p + 8));
    flSetRenderState(0x5B, (int)(p + 0x70));
    flSetRenderState(0x5C, (int)(p + 0xD8));
    flSetRenderState(1, 1);
}
typedef struct PLLIGHT {
    u8 _pad00[0x578];
    s32 col[3];                 /* 0x578 colour override of the three lights (0 = none) */
} PLLIGHT;
void Pl_light_init(PLLIGHT *pl) {
    pl->col[0] = 0;
    pl->col[1] = 0;
    pl->col[2] = 0;
}
