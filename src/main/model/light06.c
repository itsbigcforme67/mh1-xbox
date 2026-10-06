/* light06 - SLPM_654.95 0x0011E540-0x0011E858: Pl_light_set. For each of the three lights of a light set it copies the
   light (0x68 bytes) to the stack, blends its colour towards the player's colour override (pl+0x578, 0xAARRGGBB, one
   word per light; 1/5 of the way per call; no override = take the light's own colour), stores the resulting colour
   back and hands the copy to flSetRenderState(0x5A + n). Names are guesses. */
#include "types.h"

typedef struct PLLIGHT {
    u8 _pad00[0x578];
    u32 col[3];                 /* 0x578 colour override of the three lights (0 = none) */
} PLLIGHT;

typedef struct LIGHT {          /* one light of light_work (0x68 bytes) */
    f32 x00;
    f32 r;                      /* 0x04 */
    f32 g;                      /* 0x08 */
    f32 b;                      /* 0x0C */
    u8 _pad10[0x68 - 0x10];
} LIGHT;

extern u8 light_work[];
void *memcpy(void *, const void *, int);
void flSetRenderState(int, int);

void Pl_light_set(PLLIGHT *plw) {
    u32 *pl = (u32 *)plw;
    LIGHT l;
    f32 *pr = &l.r;
    f32 *pg = &l.g;
    f32 *pb = &l.b;
    LIGHT *lp = &l;
    u8 *w;
    LIGHT *src;
    s16 i;
    u32 c;
    f32 r;
    f32 g;
    f32 b;

    w = light_work + 0x150;
    for (i = 0; i < 3; i++) {
        src = (LIGHT *)(w + 8);
        memcpy(lp, src, 0x68);
        c = pl[0x578 / 4];
        if (c == 0) {
            r = src->r;
            g = src->g;
            b = src->b;
        } else {
            r = ((c >> 16) & 0xFF) / 255.0f;
            g = ((c >> 8) & 0xFF) / 255.0f;
            b = (c & 0xFF) / 255.0f;
            r = r + (src->r - r) / 5.0f;
            g = g + (src->g - g) / 5.0f;
            b = b + (src->b - b) / 5.0f;
            *pr = r;
            *pg = g;
            *pb = b;
        }
        pl[0x578 / 4] = (((u32)(255.0f * b) & 0xFF) | ((((u32)(255.0f * r) & 0xFF) << 16) | 0xFF000000 | (((u32)(255.0f * g) & 0xFF) << 8))) | 0xFF000000;
        flSetRenderState((i + 0x5A) & 0xFF, (int)lp);
        w += 0x68;
        pl++;
    }
    flSetRenderState(1, 1);
}
