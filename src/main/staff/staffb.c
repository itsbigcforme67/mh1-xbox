/* Staff roll logo sprite, 0x2909C0-0x290AE0 (logo_disp). */
#include "types.h"

extern s16 logo_tbl[];
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void flps0008();

typedef struct {
    s16 x, y, w, h;     /* 0x00 */
    u32 col;            /* 0x08 */
    s16 u, v, u2, v2;   /* 0x0C */
} SPR;

void logo_disp(x, y, no)
s16 x;
s16 y;
u8 no;
{
    SPR spr;
    int i = no * 5;
    s16 w, h;

    SetFilterMode(1);
    reload_tex(1, logo_tbl[i + 4] + 0xEA);
    SetTextureStage(logo_tbl[i + 4] + 0xEA);
    spr.x = x;
    spr.y = y;
    w = logo_tbl[i + 2];
    spr.w = w;
    h = logo_tbl[i + 3];
    spr.h = h;
    spr.col = -1;
    spr.u = logo_tbl[i];
    spr.v = logo_tbl[i + 1];
    spr.u2 = spr.u + spr.w;
    spr.v2 = spr.v + spr.h;
    flps0008(&spr, -1, h, w);
}

