/* lb_ag02 - lobby 0x005CC920-0x005CCBC0: Lb_put_button, Lb_put_icon (button / icon sprites from lb_button_tbl / lb_icon_tbl; both scale x and w by 0.8).
   The sprite descriptor IBICON holds the two texture corners as 4-byte UV structs: copying them as structs gives the original lh/lh/sh/sh pairs through pointer
   registers (member-wise s16 copies interleave the loads). The y/w/h pointers of Lb_put_icon are taken where first used and declared in the order t, ph, py, pw.
   Whole file in lb_ag.c. */
#include "lobby_f.h"
typedef struct UV { s16 u, v; } UV;
typedef struct IBICON { s16 x, y, w, h; s32 color; UV uv0, uv1; } IBICON;
typedef struct BTN { UV a, b; } BTN;
extern BTN lb_button_tbl[];
void flps0008();

s32 Lb_put_button(s32 x, s32 y, s32 idx) {
    IBICON q;
    BTN *b = &lb_button_tbl[idx];
    q.x = (s32)(0.8f * (f32)x);
    q.y = y;
    q.w = (s32)(0.8f * (f32)(b->b.u - b->a.u));
    q.h = b->b.v - b->a.v;
    if (idx == 9) {
        q.w = 0x32;
        q.h = 0x20;
    }
    q.color = -1;
    q.uv0 = b->a;
    q.uv1 = b->b;
    flps0008(&q);
    return (s16)((s16)x + (b->b.u - b->a.u));
}

void Lb_put_icon(int a, s16 b, u32 c, int d) {
    IBICON q;
    BTN *t;
    s16 *ph;
    s16 *py;
    s16 *pw;
    t = (BTN *)lb_icon_tbl + c;
    q.x = (s32)(0.8f * (f32)a);
    py = &q.y;
    *py = b;
    pw = &q.w;
    *pw = (s32)(0.8f * (f32)(t->b.u - t->a.u));
    if (*pw < 0) {
        *pw = -*pw;
    }
    ph = &q.h;
    *ph = t->b.v - t->a.v;
    switch (c) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        *pw = 0x14;
        *ph = 0x14;
        break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        q.x -= 4;
        *py -= 2;
        *pw = 0x14;
        *ph = 0x16;
        break;
    case 12:
        q.x -= 4;
        *py -= 4;
        *pw = 0x18;
        *ph = 0x18;
        break;
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
        *pw = 0x18;
        *ph = 0x1C;
        break;
    }
    q.color = d;
    q.uv0 = t->a;
    q.uv1 = t->b;
    flps0008(&q);
}
