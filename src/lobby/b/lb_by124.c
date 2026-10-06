/* lb_by124 - agent B 0x0059D890-0x0059DA34: put_main_cursor, put_main_cursor2 (menu cursor frames drawn with Put_F; the
 * 12-byte rectangle records are copied from rodata to the stack). */
#include "lobby_s.h"
typedef struct { s16 v[6]; } R12;
extern R12 lit_3380;
extern R12 lit_3382;
extern R12 lit_3397;
extern R12 lit_3399;

void put_main_cursor(int n) {
    R12 a = lit_3380;
    R12 b = lit_3382;

    if (pNet[0xC] == 0) {
        a.v[1] = n * 0x16 + 0x8C;
        a.v[3] = a.v[1] + 0x16;
        b.v[1] = a.v[1] - 2;
        b.v[3] = a.v[3] + 2;
        Put_F(&b, &b, &a.v[1], &a);
        Put_F(&a);
    }
}

void put_main_cursor2(int x, int y, s32 n) {
    R12 a = lit_3397;
    s16 t = x;
    R12 b;

    a.v[0] = t + 4;
    a.v[2] = t + 0x190;
    b = lit_3399;
    b.v[0] = t + 1;
    b.v[2] = t + 0x193;
    if (pNet[0xC] == 0) {
        a.v[1] = (s16)y + 0x3C + n * 0x16;
        a.v[3] = a.v[1] + 0x16;
        b.v[1] = a.v[1] - 2;
        b.v[3] = a.v[3] + 2;
        Put_F(&b, &a.v[3], n, &a.v[1]);
        Put_F(&a);
    }
}
