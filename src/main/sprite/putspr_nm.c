/* NONMATCHING (not built). Draw_square (0x0015B810): the original keeps the addresses of three
 * fields of the local line struct in saved registers and extends the scaled values to s16 once;
 * about 76 of 78 instructions differ, logic is believed equivalent. */
#include "types.h"
typedef struct PUT_F { s16 x0, y0; s16 x1, y1; s32 col; } PUT_F;
void flps0002(PUT_F *);

/* Outline of a rectangle: four calls of the line primitive. */
void Draw_square(int x, int y, int w, int h, u32 col) {
    PUT_F l;
    s16 sw = 0.8f * w;
    s16 yb = (s16)y + (s16)h;
    s16 sx = 0.8f * x;

    l.col = col;
    l.x0 = sx;
    l.y0 = y;
    l.x1 = sx;
    l.y1 = yb;
    flps0002(&l);
    l.x1 = sx + sw;
    l.x0 = l.x1;
    flps0002(&l);
    l.y1 = y;
    l.y0 = y;
    l.x0 = sx;
    flps0002(&l);
    l.y1 = yb;
    l.y0 = yb;
    flps0002(&l);
}
