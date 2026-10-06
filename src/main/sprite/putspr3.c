/* Put_sprite_rotate (SLPM_654.95 main 0x0015B300-0x0015B648): draws a sprite quad (SPR: x, y, w, h, colour, u, v, u2, v2)
 * at 0.8 horizontal scale (the 640 to 512 squeeze). rot 0/1: one flps0008 sprite (rot 0 swaps u and u2, i.e. mirrors it);
 * rot 2/3: the quad as two flps000C triangles with the texture turned a quarter. Field names are guesses from use. */
#include "types.h"
typedef struct SPR { s16 x, y, w, h; u32 col; s16 u, v, u2, v2; } SPR;
typedef struct TRI { s16 x0, y0, x1, y1, x2, y2; u32 col; s16 u0, v0, u1, v1, u2, v2; } TRI;
void flps0008(SPR *);
void flps000C(TRI *);

void Put_sprite_rotate(SPR *p, s8 rot) {
    TRI t;
    SPR s;
    s16 x0, y0, x1, y1, u0, u1, v0, v1;

    if (rot < 2) {
        s = *p;
        s.x = 0.8f * p->x;
        s.w = 0.8f * p->w;
        if (rot == 0) {
            s.u = p->u2;
            s.u2 = p->u;
        }
        flps0008(&s);
        return;
    }
    x0 = 0.8f * p->x;
    x1 = 0.8f * (p->x + p->w);
    y0 = p->y;
    y1 = p->y + p->h;
    u0 = p->u;
    u1 = p->u2;
    v0 = p->v;
    v1 = p->v2;
    t.col = p->col;
    switch (rot) {
    case 2:
        t.x0 = t.x1 = x0;
        t.x2 = x1;
        t.y0 = y0;
        t.y1 = t.y2 = y1;
        t.u0 = u1;
        t.u1 = t.u2 = u0;
        t.v0 = t.v1 = v0;
        t.v2 = v1;
        flps000C(&t);
        t.x0 = t.x1 = x1;
        t.x2 = x0;
        t.y0 = y1;
        t.y1 = t.y2 = y0;
        t.u0 = u0;
        t.u1 = t.u2 = u1;
        t.v0 = t.v1 = v1;
        t.v2 = v0;
        flps000C(&t);
        break;
    case 3:
        t.x0 = t.x1 = x0;
        t.x2 = x1;
        t.y0 = y0;
        t.y1 = t.y2 = y1;
        t.u0 = u0;
        t.u1 = t.u2 = u1;
        t.v0 = t.v1 = v1;
        t.v2 = v0;
        flps000C(&t);
        t.x0 = t.x1 = x1;
        t.x2 = x0;
        t.y0 = y1;
        t.y1 = t.y2 = y0;
        t.u0 = u1;
        t.u1 = t.u2 = u0;
        t.v0 = t.v1 = v0;
        t.v2 = v1;
        flps000C(&t);
        break;
    }
}
