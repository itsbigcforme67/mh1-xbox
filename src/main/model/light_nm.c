/* light_nm - SLPM_654.95 0x0011DB04-0x0011E9D8 (light_init.s) small parts: light_set, light_change_normal, Pl_light_init,
   release_tex_all; get_mdlw_ptr (0x00123C70). light_work: 2 sets of 0x140 bytes (3 lights of 0x68 starting at +0x18). Working file. */
#include "types.h"
#include "game.h"

extern GAME_W game_w;
extern u8 light_work[];
extern u8 pl_light_tbl[];       /* per stage 0x14 bytes: pointers to light colour/direction rows */
extern s32 mem_tex[];
extern u8 *mdlw_heap_area;

void flSetRenderState(int, int);
int flReleasePaletteHandle(u32);
int flReleaseTextureHandle(u16);

u8 *get_mdlw_ptr(int n) {
    int a = (int)mdlw_heap_area;
    return (u8 *)(a + (n << 7));
}

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

void light_change_normal(int n) {
    u8 *w = light_work + n * 0x140;
    u8 *tbl = pl_light_tbl + 8;
    f32 *s;

    s = *(f32 **)(tbl + game_w.stage * 0x14);
    *(f32 *)(w + 0x1C) = s[0];
    *(f32 *)(w + 0x20) = s[1];
    s += 2;
    *(f32 *)(w + 0x24) = s[0];
    s = (f32 *)(*(u8 **)(tbl + game_w.stage * 0x14) + 0x10);
    *(f32 *)(w + 0x84) = s[0];
    *(f32 *)(w + 0x88) = s[1];
    s += 2;
    *(f32 *)(w + 0x8C) = s[0];
    s = (f32 *)(*(u8 **)(tbl + game_w.stage * 0x14) + 0x20);
    *(f32 *)(w + 0xEC) = s[0];
    *(f32 *)(w + 0xF0) = s[1];
    s += 2;
    *(f32 *)(w + 0xF4) = s[0];
}

/* pl_light_change (0x0011E280, 340 bytes): logic complete, 49/85 off (the original keeps the stage 12/13/14/28/30 test as
   five separate compares and three separate copy blocks; this form gets the layout but not the registers). Not linked. */
typedef struct PLLC {
    u8 _pad00[0x613];
    u8 near_em;                 /* 0x613 near-monster light flag */
    u8 _pad614[0x710 - 0x614];
    f32 *tbl;                   /* 0x710 own light rows (0 = none) */
} PLLC;

extern f32 *pl_light_tbl2[];

void pl_light_change(PLLC *pl, int n) {
    u8 stg;
    int off;
    int i;
    f32 *dst;
    f32 *src;
    u8 *w;

    i = 0;
    off = 0;
    w = light_work + n * 0x140 + 0x10;
    do {
        dst = (f32 *)(w + 8);
        if (pl->near_em != 0) {
            stg = game_w.stage;
            if (stg == 12) goto near;
            if (stg == 13) goto near;
            if (stg == 14) goto near;
            if (stg == 28) goto near;
            if (stg == 30) goto near;
        }
        if (pl->tbl != 0) {
            if (i == 2) goto stage;
            src = (f32 *)((u8 *)pl->tbl + off);
            dst[1] = src[0];
            dst[2] = src[1];
            src += 2;
            dst[3] = src[0];
        } else {
stage:
            src = (f32 *)(*(u8 **)(pl_light_tbl + game_w.stage * 0x14 + 8) + off);
            dst[1] = src[0];
            dst[2] = src[1];
            src += 2;
            dst[3] = src[0];
        }
        goto tail;
near:
        src = pl_light_tbl2[game_w.stage];
        dst[1] = src[0];
        dst[2] = src[1];
        src += 2;
        dst[3] = src[0];
tail:
        i++;
        w += 0x68;
        off += 0x10;
    } while (i < 3);
}
