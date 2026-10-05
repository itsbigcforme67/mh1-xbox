/* Sprite put helpers. SLPM_654.95 0x0015B300-0x0015BBE0 (g_Put_sprite_rotate):
 * screen sprites authored for 640 wide are scaled by 0.8 (512 wide PS2 frame)
 * before being handed to the flps* primitives. */
#include "types.h"

typedef struct PUT_F {          /* 12 bytes: flat rectangle, corners (x0,y0)-(x1,y1) */
    s16 x0, y0;
    s16 x1, y1;
    s32 col;
} PUT_F;

typedef struct PUT_2TF {        /* 20 bytes: textured rectangle */
    s16 x0, y0;
    s16 x1, y1;
    s32 a;
    s32 b;
    s32 c;
} PUT_2TF;

void flps0004(PUT_F *);
void flps0008(PUT_2TF *);
void flps0002(PUT_F *);

void Put_2TF(PUT_2TF *p) {
    PUT_2TF q = *p;

    q.x0 = 0.8f * q.x0;
    q.x1 = 0.8f * q.x1;
    flps0008(&q);
}

void Put_F(PUT_F *p) {
    PUT_F q = *p;

    q.x0 = 0.8f * q.x0;
    q.x1 = 0.8f * q.x1;
    flps0004(&q);
}

void Paint_square(int x, int y, int w, int h, u32 col) {
    PUT_F q;

    q.col = col;
    q.x0 = 0.8f * x;
    q.x1 = 0.8f * ((s16)x + (s16)w);
    q.y1 = (s16)y + (s16)h;
    q.y0 = y;
    flps0004(&q);
}

