/* lb_nt02 - agent C 0x005C1D50-0x005C1F18: net_time_move (online play-time clock widget state machine; nested switches, ladder = reverse label order). */
#include "lobby_a.h"
extern s32 net_time_flag;
typedef struct { u8 pad0000[0x1]; s8 x0001; u8 pad0002[0x2]; u8 x0004; u8 x0005; u8 pad0006[0x12]; u8 *x0018; } ARG_net_time_move_arg0;
void net_time_str(f32);
void net_time_move(ARG_net_time_move_arg0 *arg0) {
    u8 st;
    u8 *w;
    u8 sub;
    s16 t;

    st = arg0->x0004;
    w = arg0->x0018;
    switch (st) {
    case 0:
        sub = arg0->x0005;
        switch (sub) {
        case 0:
            arg0->x0005 = sub + 1;
            *(s16 *)(w + 4) = 0x78;
            break;
        case 1:
            if (F(s8, (u8 *)cw, 0x2C30) != 0) {
                arg0->x0005 = 0;
            } else {
                t = *(s16 *)(w + 4) - 1;
                *(s16 *)(w + 4) = t;
                if (t == 0) {
                    arg0->x0004 = 1;
                    arg0->x0005 = 0;
                } else if (*(u16 *)0x3F3714 != 0) {
                    arg0->x0005 = 0;
                }
            }
            break;
        }
        break;
    case 1:
        sub = arg0->x0005;
        switch (sub) {
        case 0:
            arg0->x0005 = sub + 1;
            *(s16 *)(w + 4) = 0x10;
            arg0->x0001 = 1;
        case 1:
            t = *(s16 *)(w + 4) - 1;
            *(s16 *)(w + 4) = t;
            if (t == 0) {
                arg0->x0005 = arg0->x0005 + 1;
            }
            net_time_str(1.0f);
            break;
        case 2:
            if (F(s8, (u8 *)cw, 0x2C30) != 0) {
                arg0->x0004 = 0;
                arg0->x0005 = 0;
                arg0->x0001 = 0;
            } else {
                if (*(u16 *)0x3F3714 != 0) {
                    arg0->x0004 = 0;
                    arg0->x0005 = 0;
                    arg0->x0001 = 0;
                }
                net_time_str(1.0f);
            }
            break;
        }
        break;
    case 2:
        break;
    }
    if (net_time_flag == 0) {
        cnWrap_PushWork(arg0);
    }
}
