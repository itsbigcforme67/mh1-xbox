/* lb_pz16 - lobby.bin 0x0059CCA0-0x0059CFE4: put_mail_input_square (mail input boxes: two frames, selected one highlighted, cursor). q[] is the Put_F rectangle, p1..p3 and pc are kept as pointers into it. */
#pragma readonly_strings on
#include "lbui_proto.h"
void put_main_cursor();
void put_main_cursor2();
void put_mail_input_square(a, x, y)
LB_NETW *a;
int x;
int y;
{
    int f;
    s16 *p2;
    s16 *p3;
    u32 *pc;
    int sx;
    int sy;
    s16 *p1;
    s16 q[6];

    f = 0;
    if (a->x0C == 0) {
        u8 t = a->step;
        if (t < 7) {
            if ((*(u8 *)&a->x0A) == 0) {
                f = 1;
            }
        } else {
            switch ((*(u8 *)&a->x0A)) {
            case 0:
                f = 2;
                break;
            case 1:
                f = 1;
                break;
            }
        }
        if (t == 5 || t == 6) {
            if (a->sel == 4) {
                f = 0;
            }
        }
        sx = (s16)x + 0x33;
        sy = (s16)y;
        q[0] = sx;
        p2 = &q[2];
        *p2 = q[0] + 0x151;
        p1 = &q[1];
        *p1 = sy + 0x2A;
        p3 = &q[3];
        *p3 = *p1 + 0x16;
        if (f == 2) {
            pc = (u32 *)&q[4];
            *pc = 0xFFAA8820;
        } else {
            pc = (u32 *)&q[4];
            *pc = 0xFF501515;
        }
        if (a->step > 7) {
            Put_F(q);
        }
        q[0] += 3;
        *p2 -= 3;
        *p1 += 2;
        *p3 -= 2;
        if (f == 2) {
            *pc = 0xFF4A2020;
        } else {
            *pc = 0xFF2A0000;
        }
        if (a->step > 7) {
            Put_F(q);
        }
        q[0] = sx;
        *p2 = q[0] + 0x151;
        *p1 = sy + 0x40;
        *p3 = *p1 + 0x58;
        if (f == 1) {
            *pc = 0xFFAA8820;
        } else {
            *pc = 0xFF501515;
        }
        Put_F(q);
        q[0] += 3;
        *p2 -= 3;
        *p1 += 2;
        *p3 -= 2;
        if (f == 1) {
            *pc = 0xFF4A2020;
        } else {
            *pc = 0xFF2A0000;
        }
        Put_F(q);
        if (((LB_CW *)cw)->x35D5 != 0) {
            if (a->step < 6) {
                if ((*(u8 *)&a->x0A) == 1) {
                    put_main_cursor2(0xD8, 0x38, 7);
                }
            } else if ((*(u8 *)&a->x0A) == 2) {
                put_main_cursor2(0xD8, 0x38, 7);
            }
        } else if (a->step < 6) {
            if ((*(u8 *)&a->x0A) == 1) {
                put_main_cursor(7);
            }
        } else if ((*(u8 *)&a->x0A) == 2) {
            put_main_cursor(7);
        }
    }
}
