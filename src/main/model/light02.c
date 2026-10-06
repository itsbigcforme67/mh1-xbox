/* light02 - release all textures (SLPM_654.95 0x0011E860-0x0011E904): release_tex_all. Whole file in light_nm.c. */
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
typedef struct PLLIGHT {
    u8 _pad00[0x578];
    s32 col[3];                 /* 0x578 colour override of the three lights (0 = none) */
} PLLIGHT;
void release_tex_all(void) {
    int i;
    s32 *p;

    for (i = 0, p = mem_tex; i < 0x159; i++, p++) {
        if (*p != 0) {
            if ((*p & 0xFFFF0000) != 0 && flReleasePaletteHandle((*p & 0xFFFF0000) >> 16) == 1) {
                *p = (u16)*p;
            }
            if (flReleaseTextureHandle(*p) == 1) {
                *p &= 0xFFFF0000;
            }
        }
    }
}
