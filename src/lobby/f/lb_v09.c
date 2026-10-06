/* lb_v09 - village drawing helpers 0x005D7C30-0x005D7D60: Lb_draw_square (rectangle outline from four flps0002 lines, x and w scaled by 0.8). */
#include "lobby_f.h"
void flps0002();
void Lb_draw_square(int x, int y, int w, int h, u32 col, int scale_w) {
    s16 *p2;
    s16 *p3;
    s16 *p1;
    int y1;
    s16 sx;
    s16 q[6];
    *(u32 *)&q[4] = col;
    sx = (s16)(s32)(0.8f * (f32)x);
    if (scale_w != 0) {
        w = (s16)(s32)(0.8f * (f32)w);
    }
    p2 = &q[2];
    p1 = &q[1];
    y1 = (s16)y + (s16)h;
    *p2 = sx;
    q[0] = sx;
    *p1 = y;
    p3 = &q[3];
    *p3 = y1;
    flps0002(q);
    q[0] = *p2 = (s16)(sx + (s16)w);
    flps0002(q);
    *p1 = *p3 = y;
    q[0] = sx;
    flps0002(q);
    *p1 = *p3 = y1;
    flps0002(q);
}
